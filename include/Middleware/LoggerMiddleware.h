#ifndef LOGGERMIDDLEWARE_H
#define LOGGERMIDDLEWARE_H

#include "Middleware.h"

// Logs every request/response pair with method, path, status, and timing
class LoggerMiddleware : public Middleware {
    public:
        void handle(HttpRequest& req, HttpResponse& res, std::function next) override;
};

#endif