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