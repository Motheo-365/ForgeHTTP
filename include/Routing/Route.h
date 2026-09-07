#ifndef ROUTE_H
#define ROUTE_H

#include <functional>
#include <map>
#include <iostream>

#include "HTTP/HttpMethod.h"

using Handler = std::function<HttpResponse(HttpRequest&)>;

// One registered endpint: a method, a path pattern, and the handler to invoke on a match.
class Route {
    public:
        Route(HttpMethod method, const std::string& pattern, Handler handler);
        
        /*
            Returns whether the given concrete path matches this route's pattern, including patterns containing
            :param segments (e.g. /users/:id matches /users/42 but not /users or /users/42/edit).
        */
        bool matches(const std::string& path) const;

        /*
            Given a path already confirmed to match, returns the named parameters pulled out of it, e.g. {"id":"42"}.
        */
        std::map<std::string, std::string> extractParams(const std::string& path) const;

        HttpMethod getMethod() const;
        
    private:
        HttpMethod method;
        std::string pattern;
        Handler handler;
};

#endif