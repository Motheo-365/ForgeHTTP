#ifndef CORSMIDDLEWARE_H
#define CORSMIDDLEWARE_H

#include "Middleware.h"

// Adds permissive cross-origin headers so the react dashboard can call the API from a different origin during development.
class CorsMiddleware : public Middleware {
    public:
    void handle(HttpRequest& req, HttpResponse& res, std::function<void()> next) override;
};

#endif