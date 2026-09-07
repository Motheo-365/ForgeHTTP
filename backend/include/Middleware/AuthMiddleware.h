#ifndef AUTHMIDDLEWARE_H
#define AUTHMIDDLEWARE_H

#include "Middleware.h"

// Rejects requests that dont carry valid credentials, before they reach the router.
class AuthMiddleware : public Middleware {
    public:
        void handle(HttpRequest& req, HttpResponse& res, std::function next) override;
};

#endif