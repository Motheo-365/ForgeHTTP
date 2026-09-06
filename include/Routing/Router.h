#ifndef ROUTER_H
#define ROUTER_H

#include <vector>

// Holds every registered Route and finds the one that matches an incoming request.
class Router{
    public:
        void get(const string& path, Handler handler);
        void post(const string& path, Handler handler);
        void put(const string& path, Handler handler);
        void del(const string& push, Handler handler);
        Route* match(const HttpRequest& request);

    private:
        vector<Route> routes;
};

#endif