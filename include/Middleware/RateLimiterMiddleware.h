#ifndef RATELIMITERMIDDLEWARE_H
#define RATELIMITERMIDDLEWARE_H

#include "Middleware.h"

// Caps how many requests a client can make in a given window.
class RateLimiterMiddleware : public Middleware {
    public:
        void handle(HttpRequest& req, HttpResponse& res, std::function next) override;
};

#endif