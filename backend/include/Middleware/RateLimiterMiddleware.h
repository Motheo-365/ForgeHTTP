#ifndef RATELIMITERMIDDLEWARE_H
#define RATELIMITERMIDDLEWARE_H

#include "Middleware.h"
#include <unordered_map>
#include <string>

// Caps how many requests a client can make in a given window.
class RateLimiterMiddleware : public Middleware {
    public:
        RateLimiterMiddleware (int limit);
        /*
            Tracks request count per client (e.g. by IP), and if the caller has exceed the configure threshold, sets a 429 response and returns without calling next().
        */
        void handle(HttpRequest& req, HttpResponse& res, std::function<void()> next) override;

    private:
        std::unordered_map<std::string, int> requestCounts;
        int limit;
};

#endif