#include "Middleware/RateLimiterMiddleware.h"

RateLimiterMiddleware::RateLimiterMiddleware(int limit)
    : limit(limit) {}

void RateLimiterMiddleware::handle(HttpRequest& req, HttpResponse& res, std::function<void()> next) {
    requestCounts[req.getClientAddress()]++;
    
    if (requestCounts[req.getClientAddress()] > limit) {
        res.setStatusCode(429);
        res.setHeader("Content-Type", "application");
        res.setBody("{\"error\":\"Too Many Requests\"}");
        return;
    }

    next();
}

void RateLimiterMiddleware::setLimit(std::size_t newLimit) {
    limit = newLimit;
}