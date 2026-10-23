#ifndef RATELIMITERMIDDLEWARE_H
#define RATELIMITERMIDDLEWARE_H

#include "Middleware.h"

#include <cstddef>
#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>

class RateLimiterMiddleware : public Middleware {

public:
    explicit RateLimiterMiddleware(std::size_t limit);

    /**
     * Tracks request count per client within a 60-second window.
     * If the caller exceeds the configured threshold, sets a 429 response
     * and returns without calling next().
     */
    void handle(HttpRequest& req, HttpResponse& res, std::function<void()> next) override;

    void setLimit(std::size_t limit);

private:
    struct ClientRateLimit {
        std::size_t count = 0;
        std::chrono::steady_clock::time_point windowStart;
    };

    std::unordered_map<std::string, ClientRateLimit> requestCounts;
    std::mutex mutex;
    std::size_t limit;
    std::chrono::seconds window{60};
};

#endif