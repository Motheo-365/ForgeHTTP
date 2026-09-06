#ifndef ROUTE_H
#define ROUTE_H

#include <function>

// One registered endpint: a method, a path pattern, and the handler to invoke on a match.
class Route {
    public:
        bool matches(const string& path) const;
        map extractParams(const string& path) const;

    private:
        HttpMethod method;
        string pattern;
        function<HttpResponse(HttpRequest&)> handler;
};

#endif