#ifndef LOGGERMIDDLEWARE_H
#define LOGGERMIDDLEWARE_H

#include "Middleware.h"
#include <atomic>

// Logs every request/response pair with method, path, status, and timing
class LoggerMiddleware : public Middleware {
    public:
        void setEnabled(bool enabled);
        /*
            Record a start timestamp, 
            call next() to let the rest of the chain (and the eventual controller) run,
            print one log line once res has been populated.
        */
        void handle(HttpRequest& req, HttpResponse& res, std::function<void()> next) override;

    private:
        std::atomic<bool> enabled{true};
};

#endif