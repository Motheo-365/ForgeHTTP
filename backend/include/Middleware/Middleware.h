#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#include "HTTP/HttpResponse.h"
#include "HTTP/HttpRequest.h"
#include <functional>

// Abstract base class for every middleware link in the chain.
class Middleware{
    public:
        virtual void handle(HttpRequest& req, HttpResponse& res, std::function<void()> next) = 0;
        Middleware* setNext(Middleware* m);

    private:
        Middleware* next;
};

#endif