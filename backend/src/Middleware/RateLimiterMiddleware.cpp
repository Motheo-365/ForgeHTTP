#include "Middleware/RateLimiterMiddleware.h"

void RateLimiterMiddleware::handle(HttpRequest& req, HttpResponse& res, std::function<void()> next) {
    requestCounts[req.getClientAddress()]++;
    
    if (requestCount > limit) {
        res.status(429);
        res.setHeader("Content-Type", "application");
        res.body = "{\"error\":\"Too Many Requests\"}";
        return;
    }

    next();
}