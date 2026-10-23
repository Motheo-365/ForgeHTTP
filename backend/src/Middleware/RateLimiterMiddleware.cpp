#include "Middleware/RateLimiterMiddleware.h"

RateLimiterMiddleware::RateLimiterMiddleware(std::size_t limit)
    : limit(limit) {}

void RateLimiterMiddleware::handle(
    HttpRequest& req,
    HttpResponse& res,
    std::function<void()> next
) {
    const std::string client = req.getClientAddress();
    const auto now = std::chrono::steady_clock::now();

    bool exceeded = false;
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (requestCounts.find(client) == requestCounts.end() &&
            requestCounts.size() >= 4096) {
            auto oldest = requestCounts.begin();
            for (auto it = requestCounts.begin(); it != requestCounts.end(); ++it) {
                if (it->second.windowStart < oldest->second.windowStart) {
                    oldest = it;
                }
            }
            requestCounts.erase(oldest);
        }
        auto& rateLimit = requestCounts[client];
        if (rateLimit.count == 0 || now - rateLimit.windowStart >= window) {
            rateLimit.count = 0;
            rateLimit.windowStart = now;
        }
        exceeded = ++rateLimit.count > limit;
        if (requestCounts.size() > 2048) {
            for (auto it = requestCounts.begin(); it != requestCounts.end();) {
                if (now - it->second.windowStart >= window) {
                    it = requestCounts.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
    if (exceeded) {
        res.setStatusCode(429);
        res.setHeader("Content-Type", "application/json");
        res.setHeader("Retry-After", "60");
        res.setBody("{\"error\":\"Too Many Requests\"}");
        return;
    }

    next();
}

void RateLimiterMiddleware::setLimit(std::size_t newLimit) {
    std::lock_guard<std::mutex> lock(mutex);
    limit = newLimit;
}