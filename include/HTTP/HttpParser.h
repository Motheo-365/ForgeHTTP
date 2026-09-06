#ifndef HTTPPARSER_H
#define HTTPPARSER_H

#include "HttpRequest.h"

class HttpParser : public HttpRequest {
    public:
        static HttpRequest parse(const string& raw);

    private:
        map parseHeaders(const string& raw);
        map parseQueryString(const string& path);
};

#endif