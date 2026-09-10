#include "Middleware/CorsMiddleware.h"


void CorsMiddleware::handle(HttpRequest& req, HttpResponse& res, std::function<void()> next) {
    // CORS headers
    res.setHeader("Access-Control-Allow-Origin", "*");
    res.setHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, PATCH");
    res.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");

    // Handle CORS preflight request
    if (req.getMethod() == HttpMethod::OPTIONS) {
        res.setStatusCode(204);
        return;
    }

    next();
}