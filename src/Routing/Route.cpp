#include "Routing/Route.h"
#include <sstream>
#include <vector>

Route::Route(HttpMethod method, const std::string& pattern, Handler handler) : method(method), pattern(pattern), handler(handler) {}

bool Route::matches(const std::string& path) const {
    std::stringstream patternStream(pattern);
    std::stringstream pathStream(path);

    std::string patternSegment;
    std::string pathSegment;

    while (std::getline(patternStream, patternSegment, '/')) {
        if (!std::getline(pathStream, pathSegment, '/')) {
            return false;
        }

        if (!patternSegment.empty() && patternSegment[0] == ':') {
            continue;
        }

        if (patternSegment != pathSegment) {
            return false;
        }
    }

    if (std::getline(pathStream, pathSegment, '/')) {
        return false;
    }

    return true;
}

std::map<std::string, std::string>
Route::extractParams(const std::string& path) const {
    std::map<std::string, std::string> params;

    std::stringstream patternStream(pattern);
    std::stringstream pathStream(path);

    std::string patternSegment;
    std::string pathSegment;

    while (std::getline(patternStream, patternSegment, '/') && std::getline(pathStream, pathSegment, '/')) {
        if (!patternSegment.empty() && patternSegment[0] == ':') {
            std::string parameterName = patternSegment.substr(1);
            params[parameterName] = pathSegment;
        }
    }

    return params;
}

HttpMethod Route::getMethod() const {
    return method;
}