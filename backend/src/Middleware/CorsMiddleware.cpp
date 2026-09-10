#include "Middleware/CorsMiddleware.h"


void CorsMiddleware::handle(HttpRequest& req, HttpResponse& res, std::function<void()> next) {
    // Handle CORS preflight request
    if (req.getMethod() == HttpMethod::OPTIONS) {
        res.setHeader("Access-Control-Allow-Origin", "*");
        res.setHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, PATCH, OPTIONS");
        res.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.setStatusCode(204);
        return;
    }
    
    next();

    // CORS headers
    res.setHeader("Access-Control-Allow-Origin", "*");
    res.setHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, PATCH, OPTOINS");
    res.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
}