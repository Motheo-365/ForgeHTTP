#include "Routing/Router.h"

void Router::get (const std::string& path, Handler handler) {
    routes.emplace_back(HttpMethod::GET, path, handler);
}

void Router::post(const std::string& path, Handler handler) {
    routes.emplace_back(HttpMethod::POST, path, handler);
}

void Router::put(const std::string& path, Handler handler) {
    routes.emplace_back(HttpMethod::PUT, path, handler);
}

void Router::del(const std::string& path, Handler handler) {
    routes.emplace_back(HttpMethod::DELETE, path, handler);
}

Route* Router::match(const HttpRequest& request) {
    for (Route& route : routes) {
        if (route.getMethod() == request.getMethod() && route.matches(request.getPath())) {
            return &route;
        }
    }

    return nullptr;
}