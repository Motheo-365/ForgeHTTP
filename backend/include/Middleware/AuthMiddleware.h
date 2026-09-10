#ifndef AUTHMIDDLEWARE_H
#define AUTHMIDDLEWARE_H

#include "Middleware.h"

// Rejects requests that dont carry valid credentials, before they reach the router.
class AuthMiddleware : public Middleware {
    public:
        /*
            Checks for valid Authorization header.
            If missing or invalid, sets a 401 response and returns without calling next()/
            Otherwise calls next() to continue the chain.
        */
        void handle(HttpRequest& req, HttpResponse& res, std::function<void()> next) override;
};

#endif