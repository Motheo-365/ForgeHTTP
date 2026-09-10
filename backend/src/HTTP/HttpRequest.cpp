#include "HTTP/HttpRequest.h"

#include <algorithm>
#include <cctype>
#include <string>

std::string HttpRequest::getHeader(const std::string& key) const {
    for (const auto& header: headers) {
       std::string headerName = header.first;
       std::string requestedName = key;

        transform(
            headerName.begin(),
            headerName.end(),
            headerName.begin(),
            [](unsigned char c) {
                return tolower(c);
            }
        );

        transform(
            requestedName.begin(),
            requestedName.end(),
            requestedName.begin(),
            [](unsigned char c) {
                return tolower(c);
            }
        );

        if (headerName == requestedName) {
            return header.second;
        }
    }

    return "";
}

std::string HttpRequest::getQueryParam(const std::string& key) const {
    // Search query map for requested parameter
    auto it = query.find(key);

    if (it != query.end()) { // If it exists, return its value
        return it->second;
    }

    return "";
}

std::string HttpRequest::getPath() const {
    return path;
}

HttpMethod HttpRequest::getMethod() const {
    return method;
}

std::string HttpRequest::getVersion() const {
    return version;
}

std::string HttpRequest::getBody() const {
    return body;
}

std::string HttpRequest::getClinetAddress() const {
    return body;
}