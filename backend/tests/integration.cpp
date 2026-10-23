#include "Server/Server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {
constexpr const char* ApiToken = "portfolio-test-token";

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

int findUnusedPort() {
    const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) throw std::runtime_error("Could not create test socket");

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = 0;
    if (::bind(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        ::close(fd);
        throw std::runtime_error("Could not bind test socket");
    }

    socklen_t length = sizeof(address);
    if (getsockname(fd, reinterpret_cast<sockaddr*>(&address), &length) < 0) {
        ::close(fd);
        throw std::runtime_error("Could not read test port");
    }
    const int port = ntohs(address.sin_port);
    ::close(fd);
    return port;
}

std::string exchangeHttp(int port, const std::string& request, bool closeWrite = false) {
    const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) throw std::runtime_error("Could not create client socket");

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(static_cast<std::uint16_t>(port));

    bool connected = false;
    for (int attempt = 0; attempt < 100; ++attempt) {
        if (::connect(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == 0) {
            connected = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    if (!connected) {
        ::close(fd);
        throw std::runtime_error("Could not connect to test server");
    }

    std::size_t sent = 0;
    while (sent < request.size()) {
        const ssize_t count = ::send(fd, request.data() + sent, request.size() - sent, 0);
        if (count <= 0) {
            ::close(fd);
            throw std::runtime_error("Could not send test request");
        }
        sent += static_cast<std::size_t>(count);
    }
    if (closeWrite) ::shutdown(fd, SHUT_WR);

    std::string response;
    char buffer[4096];
    for (;;) {
        const ssize_t count = ::recv(fd, buffer, sizeof(buffer), 0);
        if (count == 0) break;
        if (count < 0) {
            ::close(fd);
            throw std::runtime_error("Could not read test response");
        }
        response.append(buffer, static_cast<std::size_t>(count));
    }
    ::close(fd);
    return response;
}

std::string request(const std::string& method, const std::string& path,
                    const std::string& body = {}, bool authenticated = false) {
    std::string result = method + " " + path + " HTTP/1.1\r\nHost: localhost\r\n";
    if (authenticated) {
        result += std::string("Authorization: Bearer ") + ApiToken + "\r\n";
    }
    if (!body.empty() || method == "POST" || method == "PUT") {
        result += "Content-Type: application/json\r\nContent-Length: " +
            std::to_string(body.size()) + "\r\n";
    }
    result += "Connection: close\r\n\r\n";
    result += body;
    return result;
}

bool hasStatus(const std::string& response, const std::string& status) {
    return response.rfind("HTTP/1.1 " + status, 0) == 0;
}

void runSuite(int port) {
    const std::string health = exchangeHttp(port, request("GET", "/health"));
    require(hasStatus(health, "200 OK"), "health endpoint should return 200");
    require(health.find("Connection: close\r\n") != std::string::npos,
            "responses should document close-after-response behavior");

    const std::string missing = exchangeHttp(port, request("GET", "/not-a-route"));
    require(hasStatus(missing, "404 Not Found"), "unknown route should return 404");

    const std::string wrongMethod = exchangeHttp(port, request("POST", "/health", "{}"));
    require(hasStatus(wrongMethod, "405 Method Not Allowed"),
            "known route with unsupported method should return 405");
    require(wrongMethod.find("Allow: GET\r\n") != std::string::npos,
            "405 should include Allow");

    const std::string unauthorized =
        exchangeHttp(port, request("GET", "/api/users"));
    require(hasStatus(unauthorized, "401 Unauthorized"),
            "protected routes should reject missing credentials");

    const std::string publicRequestHistory =
        exchangeHttp(port, request("GET", "/api/requests"));
    require(hasStatus(publicRequestHistory, "200 OK"),
            "request history should be publicly readable");

    const std::string publicConfig =
        exchangeHttp(port, request("GET", "/api/config"));
    require(hasStatus(publicConfig, "200 OK"),
            "configuration should be publicly readable");

    const std::string calculation = exchangeHttp(port, request(
        "POST", "/api/calculate",
        "{\"left\":12,\"operator\":\"*\",\"right\":3}"));
    require(hasStatus(calculation, "200 OK"),
            "calculator endpoint should be public");
    require(calculation.find("\"result\":36") != std::string::npos,
            "calculator endpoint should return the computed result");

    const std::string divisionByZero = exchangeHttp(port, request(
        "POST", "/api/calculate",
        "{\"left\":1,\"operator\":\"/\",\"right\":0}"));
    require(hasStatus(divisionByZero, "400 Bad Request"),
            "calculator should reject division by zero");

    const std::string unsupportedOperator = exchangeHttp(port, request(
        "POST", "/api/calculate",
        "{\"left\":1,\"operator\":\"^\",\"right\":2}"));
    require(hasStatus(unsupportedOperator, "400 Bad Request"),
            "calculator should reject unsupported operators");

    const std::string invalidCalculation = exchangeHttp(port, request(
        "POST", "/api/calculate", "{\"left\":\"one\",\"operator\":\"+\",\"right\":2}"));
    require(hasStatus(invalidCalculation, "400 Bad Request"),
            "calculator should reject non-numeric operands");

    const std::string protectedConfigUpdate =
        exchangeHttp(port, request("PUT", "/api/config", "{\"rateLimit\":150}"));
    require(hasStatus(protectedConfigUpdate, "401 Unauthorized"),
            "configuration changes should still require credentials");

    const std::string created = exchangeHttp(port, request(
        "POST", "/api/users", "{\"name\":\"Portfolio Tester\"}", true));
    require(hasStatus(created, "201 Created"), "creating a user should return 201");
    require(created.find("\"name\":\"Portfolio Tester\"") != std::string::npos,
            "created user response should contain persisted data");

    const std::string fetched =
        exchangeHttp(port, request("GET", "/api/users/1", {}, true));
    require(hasStatus(fetched, "200 OK"), "path parameter route should fetch a user");
    require(fetched.find("\"name\":\"Portfolio Tester\"") != std::string::npos,
            "user lookup should return the stored user");

    const std::string updated = exchangeHttp(port, request(
        "PUT", "/api/users/1", "{\"name\":\"Updated Tester\"}", true));
    require(hasStatus(updated, "200 OK"), "updating a user should return 200");
    require(updated.find("\"name\":\"Updated Tester\"") != std::string::npos,
            "updated user should return the changed data");

    const std::string deleted =
        exchangeHttp(port, request("DELETE", "/api/users/1", {}, true));
    require(hasStatus(deleted, "204 No Content"), "deleting a user should return 204");
    require(deleted.find("Content-Length:") == std::string::npos,
            "204 response must not include Content-Length");
    const std::string missingUser =
        exchangeHttp(port, request("GET", "/api/users/1", {}, true));
    require(hasStatus(missingUser, "404 Not Found"), "deleted user should return 404");

    const std::string liveConfig = exchangeHttp(port, request(
        "PUT", "/api/config",
        "{\"rateLimit\":150,\"corsEnabled\":false,"
        "\"authenticationEnabled\":false,\"loggingEnabled\":false}",
        true));
    require(hasStatus(liveConfig, "200 OK"), "supported settings should update live");
    const std::string publicUsers =
        exchangeHttp(port, request("GET", "/api/users"));
    require(hasStatus(publicUsers, "200 OK"),
            "disabling authentication should affect protected routes");
    const std::string corsDisabled = exchangeHttp(port, request("GET", "/health"));
    require(corsDisabled.find("Access-Control-Allow-Origin:") == std::string::npos,
            "disabling CORS should remove CORS response headers");
    const std::string restoreConfig = exchangeHttp(port, request(
        "PUT", "/api/config",
        "{\"corsEnabled\":true,\"authenticationEnabled\":true,"
        "\"loggingEnabled\":true}",
        true));
    require(hasStatus(restoreConfig, "200 OK"), "runtime settings should be restorable");
    const std::string restartRequired = exchangeHttp(port, request(
        "PUT", "/api/config", "{\"port\":1}", true));
    require(hasStatus(restartRequired, "400 Bad Request"),
            "port changes should clearly require restart");

    const std::string persisted = exchangeHttp(port, request(
        "POST", "/api/users", "{\"name\":\"Persisted User\"}", true));
    require(hasStatus(persisted, "201 Created"), "second user should be created");

    const std::string oversized = exchangeHttp(port,
        "POST /api/users HTTP/1.1\r\nHost: localhost\r\nContent-Length: 1048577\r\n\r\n");
    require(hasStatus(oversized, "413 Payload Too Large"),
            "oversized body should return 413 before reading the body");

    const std::string incomplete = exchangeHttp(port,
        "POST /api/users HTTP/1.1\r\nHost: localhost\r\nContent-Length: 4\r\n\r\nx",
        true);
    require(hasStatus(incomplete, "400 Bad Request"),
            "incomplete body should return 400");

    std::atomic<int> concurrentSuccesses{0};
    std::vector<std::thread> clients;
    for (int i = 0; i < 12; ++i) {
        clients.emplace_back([&]() {
            try {
                const std::string response = exchangeHttp(port, request("GET", "/health"));
                if (hasStatus(response, "200 OK")) ++concurrentSuccesses;
            } catch (...) {
            }
        });
    }
    for (std::thread& client : clients) client.join();
    require(concurrentSuccesses == 12, "concurrent clients should all receive 200");
}

int openStalledRequest(int port) {
    const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) throw std::runtime_error("Could not create stalled client socket");
    timeval timeout{};
    timeout.tv_sec = 8;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(static_cast<std::uint16_t>(port));
    if (::connect(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        ::close(fd);
        throw std::runtime_error("Could not connect stalled client");
    }
    const std::string partial = "GET /health HTTP/1.1\r\nHost:";
    if (::send(fd, partial.data(), partial.size(), 0) !=
        static_cast<ssize_t>(partial.size())) {
        ::close(fd);
        throw std::runtime_error("Could not send stalled request");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return fd;
}
}

int main() {
    ::setenv("FORGEHTTP_API_TOKEN", ApiToken, 1);
    const int port = findUnusedPort();
    const std::filesystem::path databasePath =
        std::filesystem::temp_directory_path() /
        ("forgehttp-integration-" + std::to_string(port) + ".sqlite");
    ::setenv("FORGEHTTP_DB_PATH", databasePath.string().c_str(), 1);

    Server server;
    std::exception_ptr serverError;
    std::thread serverThread([&]() {
        try {
            server.start(port);
        } catch (...) {
            serverError = std::current_exception();
        }
    });

    int result = 0;
    try {
        runSuite(port);
        std::cout << "[PASS] HTTP server integration suite\n";
    } catch (const std::exception& error) {
        std::cerr << "[FAIL] HTTP server integration suite: " << error.what() << '\n';
        result = 1;
    }

    int stalledClient = -1;
    try {
        stalledClient = openStalledRequest(port);
    } catch (const std::exception& error) {
        std::cerr << "[FAIL] Starting in-flight shutdown request: "
                  << error.what() << '\n';
        result = 1;
    }
    server.stop();
    if (stalledClient >= 0) {
        char buffer[4096];
        std::string response;
        for (;;) {
            const ssize_t count = ::recv(stalledClient, buffer, sizeof(buffer), 0);
            if (count <= 0) break;
            response.append(buffer, static_cast<std::size_t>(count));
        }
        ::close(stalledClient);
        if (!hasStatus(response, "408 Request Timeout")) {
            std::cerr << "[FAIL] In-flight request should finish with 408 on timeout\n";
            result = 1;
        }
    }
    serverThread.join();
    if (serverError != nullptr) {
        try {
            std::rethrow_exception(serverError);
        } catch (const std::exception& error) {
            std::cerr << "[FAIL] Server thread: " << error.what() << '\n';
            result = 1;
        }
    }
    try {
        UserController restartedController(databasePath.string());
        const std::string persisted = restartedController.getUsers(HttpRequest{}).toString();
        if (persisted.find("\"name\":\"Persisted User\"") == std::string::npos) {
            std::cerr << "[FAIL] SQLite data should survive controller recreation\n";
            result = 1;
        }
    } catch (const std::exception& error) {
        std::cerr << "[FAIL] SQLite persistence check: " << error.what() << '\n';
        result = 1;
    }
    std::error_code cleanupError;
    std::filesystem::remove(databasePath, cleanupError);
    return result;
}
