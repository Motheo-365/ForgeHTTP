#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <map>
#include <string>

#include "HttpMethod.h"

// A parsed, structure representation of an incoming HTTP request.
class HttpRequest {
    public:
        /*
            Returns the value of the named header, or an empty std::string if it is not present. Lookup should be case-insensitive per the HTTP spec.
        */
        std::string getHeader(const std::string& key) const;

        /*
            Returns the value of the named query parameter, or an empty std::string if absent.
        */
        std::string getQueryParam(const std::string& key) const;
        
        std::string getPath() const;
        HttpMethod getMethod() const;
        std::string getVersion() const;
        std::string getBody() const;

private:
        friend class HttpParser;
        
        /*
            Populated by HttpParser from the request line, e.g. GET /users?id=10 HTTP/1.1 becomes method=GET, path=/users, version=HTTP/1.1
        */
        HttpMethod method;
        std::string path;
        std::string version;

        /*
            All request header, keyed by name (e.g. "Host", "Accept")
        */
        std::map<std::string, std::string> headers;

        /*
            The raw request body, if any (e.g.JSON payload on a POST)
        */
        std::string body;

        /*
            Key/value pairs parsed form the quert std::string portion of the path.
        */
       std:: map<std::string , std::string> query;

        /*
            Key, value pairs extracted from a mached route patter,
            e.g. /user/:id matching /users/43 populates params["id"]="43"
        */
        std::map<std::string, std::string> params;
};

#endif