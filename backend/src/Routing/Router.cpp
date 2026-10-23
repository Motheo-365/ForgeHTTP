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

void Router::patch(const std::string& path, Handler handler) {
    routes.emplace_back(HttpMethod::PATCH, path, handler);
}

void Router::del(const std::string& path, Handler handler) {
    routes.emplace_back(HttpMethod::DELETE, path, handler);
}

Route* Router::match(HttpRequest& request) {
    for (Route& route : routes) {
        if (route.getMethod() == request.getMethod() && route.matches(request.getPath())) {
            request.params = route.extractParams(request.getPath());
            return &route;
        }
    }

    return nullptr;
}

bool Router::hasPath(const std::string& path) const {
    for (const Route& route : routes) {
        if (route.matches(path)) {
            return true;
        }
    }
    return false;
}

std::string Router::allowedMethods(const std::string& path) const {
    std::string methods;
    for (const Route& route : routes) {
        if (!route.matches(path)) {
            continue;
        }
        std::string method;
        switch (route.getMethod()) {
            case HttpMethod::GET: method = "GET"; break;
            case HttpMethod::POST: method = "POST"; break;
            case HttpMethod::PUT: method = "PUT"; break;
            case HttpMethod::DELETE: method = "DELETE"; break;
            case HttpMethod::PATCH: method = "PATCH"; break;
            case HttpMethod::OPTIONS: method = "OPTIONS"; break;
        }
        if (methods.find(method) == std::string::npos) {
            if (!methods.empty()) methods += ", ";
            methods += method;
        }
    }
    return methods;
}