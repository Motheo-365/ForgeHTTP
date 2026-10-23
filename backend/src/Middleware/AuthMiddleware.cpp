#include "Middleware/AuthMiddleware.h"

#include <utility>

AuthMiddleware::AuthMiddleware(std::string token)
    : token(std::move(token)) {}

void AuthMiddleware::handle(HttpRequest& req, HttpResponse& res, std::function<void()> next) {
    if (token.empty()) {
        res.setStatusCode(503);
        res.setHeader("Content-Type", "application/json");
        res.setBody("{\"error\":\"API authentication is not configured\"}");
        return;
    }
    const std::string authorization = req.getHeader("Authorization");
    if (authorization != "Bearer " + token) {
        res.setStatusCode(401);
        res.setHeader("Content-Type", "application/json");
        res.setHeader("WWW-Authenticate", "Bearer");
        res.setBody("{\"error\":\"Unauthorized\"}");
        return;
    }

    next();
}