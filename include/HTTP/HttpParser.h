#ifndef HTTPPARSER_H
#define HTTPPARSER_H

#include "HttpRequest.h"

class HttpParser {
    public:
        /*
            Parses the request line, headers, and body from the raw socket bytes and returns a fully populated HttpRequest. 
            Should tolerate missing trailing whitespace and both \r\n and \n line endings.

            For example:
                GET /hello?name=Motheo HTTP/1.2
                Host: localhost
                Content-Type: text/plain

                Hello
        */
        static HttpRequest parse(const std::string& raw);

    private:
        /*
            Splits the header block into a map, one entry per Key: Value line.
        */
        static map<std::string, std::string> parseHeaders(const std::string& raw);

        /*
            Splits everything after ? in the path into key/value pairs, and return the map (the caller strips the query std::string from the stored path.)
        */
        static map<std::string, std::string> parseQueryString(const std::string& path);
};

#endif