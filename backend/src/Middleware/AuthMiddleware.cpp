#include "Middleware/AuthMiddleware.h"

void AuthMiddleware::handle(HttpRequest& req, HttpResponse& res, std::function<void()> next) {
    std::string authorization = req.getHeader("Authorization");

    if (authorization.empty()) {
        res.setStatusCode(401);
        res.setBody("{\"error\":\"Unauthorized\"}");
        return;
    }

    next();
}