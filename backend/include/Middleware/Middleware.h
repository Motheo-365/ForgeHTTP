#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

// Abstract base class for every middleware link in the chain.
class Middleware{
    public:
        void handle(HttpRequest& req, HttpResponse& res, std::function next) = 0;
        Middleware* setNext(Middleware* m);

    private:
        Middleware* next;
};

#endif