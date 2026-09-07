# include "Controllers/UserController.h"
# include <nlohmann/json.hpp>

using json = nlohmann::json;

HttpResponse UserController::getUsers (const HttpRequest& req) {
    return HttpResponse::json(
        "["
        "{\"id\":1,\"name\":\"Alice\"},"
        "{\"id\":2,\"name\":\"Bob\"}"
        "]"
    );
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

        HttpResponse response = HttpResponse::json(
            "{\"id\":3,\"name\":\"" +
            data["name"].get<std::string>() +
            "\"}"
        );

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