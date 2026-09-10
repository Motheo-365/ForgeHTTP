# include "Controllers/UserController.h"
# include <nlohmann/json.hpp>

using json = nlohmann::json;

UserController::UserController() {
    users.push_back({1, "Alice"});
    users.push_back({2, "Bob"});

    nextId = 3;
}

HttpResponse UserController::getUsers (const HttpRequest&) {
    json result = json::array();

    for (const User& user: users) {
        result.push_back({
            {"id", user.id},
            {"name", user.name}
        });
    }

    return HttpResponse::json(result.dump());
}

HttpResponse UserController::createUser(const HttpRequest& req) {
    try {
        json data = json::parse(req.getBody());

        if (!data.contains("name") || !data["name"].is_string()) {
            HttpResponse response = HttpResponse::json(
                "{\"error\":\"Name is required\"}"
            );

            response.setStatusCode(400);
            return response;
        }

        User user;
        user.id = nextId++;
        user.name = data["name"].get<std::string>();

        users.push_back(user);

        json result = {
            {"id", user.id},
            {"name", user.name}
        };

        HttpResponse response =
            HttpResponse::json(result.dump());

        response.setStatusCode(201);

        return response;
    }

    catch (const json::parse_error&) {
        HttpResponse response = HttpResponse::json(
            "{\"error\":\"Invalid JSON\"}"
        );

        response.setStatusCode(400);

        return response;
    }
}