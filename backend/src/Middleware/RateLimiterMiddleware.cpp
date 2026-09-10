#include "Middleware/RateLimiterMiddleware.h"

RateLimiterMiddleware::RateLimiterMiddleware(int limit)
    : limit(limit) {}

void RateLimiterMiddleware::handle(
    HttpRequest& req,
    HttpResponse& res,
    std::function<void()> next
) {
    const std::string client = req.getClientAddress();
    const auto now = std::chrono::steady_clock::now();

    auto& rateLimit = requestCounts[client];

    if (rateLimit.count == 0) {
        rateLimit.windowStart = now;
    }

    if (now - rateLimit.windowStart >= window) {
        rateLimit.count = 0;
        rateLimit.windowStart = now;
    }

    rateLimit.count++;

    if (rateLimit.count > limit) {
        res.setStatusCode(429);
        res.setHeader("Content-Type", "application/json");
        res.setBody("{\"error\":\"Too Many Requests\"}");
        return;
    }

    next();
}

void RateLimiterMiddleware::setLimit(std::size_t newLimit) {
    limit = static_cast<int>(newLimit);
}