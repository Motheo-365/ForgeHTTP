#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#include "HTTP/HttpResponse.h"
#include "HTTP/HttpRequest.h"
#include <functional>

// Abstract base class for every middleware link in the chain.
class Middleware{
    public:
        /*
            The one method every middleware implements.
            Should perform its own work (logging, header injection, auth checks, etc.) and then
            call next() to continue down the chain --- or omit the call to short-circuit the request (e.g. rejecting an unauthenticated request before it reaches the router.)
        */
        virtual void handle(HttpRequest& req, HttpResponse& res, std::function<void()> next) = 0;
        
        /*
            Links the middleware to the next one in the chain and returns it, enabling
                a.setNext(&b)->setNext(&c);
            style chaining at server setup time.
        */
        Middleware* setNext(Middleware* m);

    private:
        Middleware* next;
};

#endif