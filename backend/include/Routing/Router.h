#ifndef ROUTER_H
#define ROUTER_H

#include "HTTP/HttpRequest.h"
#include "Route.h"

#include <vector>
#include <string>
#include <functional>

// Holds every registered Route and finds the one that matches an incoming request.
class Router{
    public:
        /*
            Registers a GET route gor the given path pattern.
        */
        void get(const std::string& path, Handler handler);

        /*
            Registers a POST route for the given path pattern
        */
        void post(const std::string& path, Handler handler);

        /*
            Registers a PUT route for the given path pattern
        */
        void put(const std::string& path, Handler handler);

        /*
            Registers a DELETE route for the given path pattern/
            Name del rather than delete because delete is a keyword.
        */
        void del(const std::string& path, Handler handler);

        /*
            Scans the registered routes for one whose method and pattern match teh request, returning a pointer to it (or nullptr if none match, which should produce a 404 response upstream)
        */
        Route* match(const HttpRequest& request);

    private:
        std::vector<Route> routes;
};

#endif