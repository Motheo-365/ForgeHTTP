#include "Controllers/UserController.h"

#include <nlohmann/json.hpp>
#include <sqlite3.h>

#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <memory>
#include <stdexcept>

using json = nlohmann::json;

namespace {
using Statement = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

HttpResponse jsonError(int status, const std::string& message) {
    HttpResponse response = HttpResponse::json(json{{"error", message}}.dump());
    response.setStatusCode(status);
    return response;
}

bool hasJsonContentType(const HttpRequest& request) {
    std::string contentType = request.getHeader("Content-Type");
    const std::size_t parameter = contentType.find(';');
    if (parameter != std::string::npos) contentType.erase(parameter);
    std::transform(contentType.begin(), contentType.end(), contentType.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return contentType == "application/json";
}

void checkSqlite(int result, sqlite3* database, const std::string& operation) {
    if (result != SQLITE_OK && result != SQLITE_DONE && result != SQLITE_ROW) {
        throw std::runtime_error(operation + ": " + sqlite3_errmsg(database));
    }
}

bool parseId(const HttpRequest& request, sqlite3_int64& id) {
    const std::string value = request.getPathParam("id");
    if (value.empty() ||
        !std::all_of(value.begin(), value.end(), [](unsigned char c) {
            return c >= '0' && c <= '9';
        })) {
        return false;
    }

    std::size_t parsed = 0;
    try {
        id = std::stoll(value, &parsed);
    } catch (const std::exception&) {
        return false;
    }
    return parsed == value.size() && id > 0;
}

std::string getName(const HttpRequest& request) {
    const json body = json::parse(request.getBody());
    if (!body.is_object() || body.size() != 1 ||
        !body.contains("name") || !body["name"].is_string()) {
        throw std::invalid_argument("A string name is required");
    }
    const std::string name = body["name"].get<std::string>();
    const bool onlyWhitespace = std::all_of(name.begin(), name.end(),
        [](unsigned char c) { return std::isspace(c); });
    if (name.empty() || onlyWhitespace || name.size() > 100) {
        throw std::invalid_argument("Name must contain between 1 and 100 bytes");
    }
    return name;
}

json userFromRow(sqlite3_stmt* statement) {
    return json{
        {"id", sqlite3_column_int64(statement, 0)},
        {"name", reinterpret_cast<const char*>(sqlite3_column_text(statement, 1))}
    };
}
}

UserController::UserController(const std::string& databasePath) {
    std::string path = databasePath;
    if (path.empty()) {
        const char* configuredPath = std::getenv("FORGEHTTP_DB_PATH");
        path = configuredPath == nullptr ? "forgehttp.db" : configuredPath;
    }
    if (path.empty()) {
        throw std::invalid_argument("Database path must not be empty");
    }

    const int openResult = sqlite3_open_v2(
        path.c_str(), &database,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
        nullptr);
    if (openResult != SQLITE_OK) {
        const std::string message = database == nullptr
            ? "Unable to open SQLite database"
            : sqlite3_errmsg(database);
        if (database != nullptr) sqlite3_close(database);
        database = nullptr;
        throw std::runtime_error("Unable to open SQLite database: " + message);
    }

    char* error = nullptr;
    const int result = sqlite3_exec(
        database,
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL CHECK(length(name) BETWEEN 1 AND 100)"
        ");",
        nullptr, nullptr, &error);
    if (result != SQLITE_OK) {
        const std::string message = error == nullptr ? sqlite3_errmsg(database) : error;
        sqlite3_free(error);
        sqlite3_close(database);
        database = nullptr;
        throw std::runtime_error("Unable to initialize users table: " + message);
    }
}

UserController::~UserController() {
    if (database != nullptr) {
        sqlite3_close(database);
    }
}

HttpResponse UserController::getUsers(const HttpRequest&) {
    std::lock_guard<std::mutex> lock(mutex);
    sqlite3_stmt* rawStatement = nullptr;
    checkSqlite(sqlite3_prepare_v2(
        database, "SELECT id, name FROM users ORDER BY id", -1,
        &rawStatement, nullptr), database, "Preparing user list query");
    Statement statement(rawStatement, sqlite3_finalize);

    json result = json::array();
    int step = SQLITE_OK;
    while ((step = sqlite3_step(statement.get())) == SQLITE_ROW) {
        result.push_back(userFromRow(statement.get()));
    }
    checkSqlite(step, database, "Reading users");
    return HttpResponse::json(result.dump());
}

HttpResponse UserController::getUser(const HttpRequest& request) {
    sqlite3_int64 id = 0;
    if (!parseId(request, id)) return jsonError(400, "Invalid user ID");

    std::lock_guard<std::mutex> lock(mutex);
    sqlite3_stmt* rawStatement = nullptr;
    checkSqlite(sqlite3_prepare_v2(
        database, "SELECT id, name FROM users WHERE id = ?", -1,
        &rawStatement, nullptr), database, "Preparing user query");
    Statement statement(rawStatement, sqlite3_finalize);
    checkSqlite(sqlite3_bind_int64(statement.get(), 1, id), database, "Binding user ID");
    const int result = sqlite3_step(statement.get());
    if (result == SQLITE_DONE) return jsonError(404, "User not found");
    checkSqlite(result, database, "Reading user");
    return HttpResponse::json(userFromRow(statement.get()).dump());
}

HttpResponse UserController::createUser(const HttpRequest& request) {
    if (!hasJsonContentType(request)) {
        return jsonError(415, "Content-Type must be application/json");
    }
    std::string name;
    try {
        name = getName(request);
    } catch (const json::exception&) {
        return jsonError(400, "Invalid JSON body");
    } catch (const std::invalid_argument& error) {
        return jsonError(400, error.what());
    }

    std::lock_guard<std::mutex> lock(mutex);
    sqlite3_stmt* rawStatement = nullptr;
    checkSqlite(sqlite3_prepare_v2(
        database, "INSERT INTO users(name) VALUES(?)", -1,
        &rawStatement, nullptr), database, "Preparing user creation");
    Statement statement(rawStatement, sqlite3_finalize);
    checkSqlite(sqlite3_bind_text(
        statement.get(), 1, name.c_str(), static_cast<int>(name.size()),
        SQLITE_TRANSIENT), database, "Binding user name");
    checkSqlite(sqlite3_step(statement.get()), database, "Creating user");

    const sqlite3_int64 id = sqlite3_last_insert_rowid(database);
    HttpResponse response =
        HttpResponse::json(json{{"id", id}, {"name", name}}.dump());
    response.setStatusCode(201);
    return response;
}

HttpResponse UserController::updateUser(const HttpRequest& request) {
    sqlite3_int64 id = 0;
    if (!parseId(request, id)) return jsonError(400, "Invalid user ID");
    if (!hasJsonContentType(request)) {
        return jsonError(415, "Content-Type must be application/json");
    }
    std::string name;
    try {
        name = getName(request);
    } catch (const json::exception&) {
        return jsonError(400, "Invalid JSON body");
    } catch (const std::invalid_argument& error) {
        return jsonError(400, error.what());
    }

    std::lock_guard<std::mutex> lock(mutex);
    sqlite3_stmt* rawStatement = nullptr;
    checkSqlite(sqlite3_prepare_v2(
        database, "UPDATE users SET name = ? WHERE id = ?", -1,
        &rawStatement, nullptr), database, "Preparing user update");
    Statement statement(rawStatement, sqlite3_finalize);
    checkSqlite(sqlite3_bind_text(
        statement.get(), 1, name.c_str(), static_cast<int>(name.size()),
        SQLITE_TRANSIENT), database, "Binding user name");
    checkSqlite(sqlite3_bind_int64(statement.get(), 2, id), database, "Binding user ID");
    checkSqlite(sqlite3_step(statement.get()), database, "Updating user");
    if (sqlite3_changes(database) == 0) return jsonError(404, "User not found");
    return HttpResponse::json(json{{"id", id}, {"name", name}}.dump());
}

HttpResponse UserController::deleteUser(const HttpRequest& request) {
    sqlite3_int64 id = 0;
    if (!parseId(request, id)) return jsonError(400, "Invalid user ID");

    std::lock_guard<std::mutex> lock(mutex);
    sqlite3_stmt* rawStatement = nullptr;
    checkSqlite(sqlite3_prepare_v2(
        database, "DELETE FROM users WHERE id = ?", -1,
        &rawStatement, nullptr), database, "Preparing user deletion");
    Statement statement(rawStatement, sqlite3_finalize);
    checkSqlite(sqlite3_bind_int64(statement.get(), 1, id), database, "Binding user ID");
    checkSqlite(sqlite3_step(statement.get()), database, "Deleting user");
    if (sqlite3_changes(database) == 0) return jsonError(404, "User not found");
    HttpResponse response;
    response.setStatusCode(204);
    return response;
}
