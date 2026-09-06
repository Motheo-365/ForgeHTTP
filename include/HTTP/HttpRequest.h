#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <map>

// A parsed, structure representation of an incoming HTTP request.
class HttpRequest {
    public:
        string getHeader(const string& key) const;
        string getQueryParam(const string& key) const;

    private:
        HttpMethod method;
        string path;
        string version;
        map<string, string> headers;
        string body;
        map<string , string> query;
        map<string, string> params;
};

#endif