#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <map>
#include <string>

// A structured HTTP response, convertible to raw wire bytes
class HttpResponse {
    public:
       static HttpResponse json(string data);
       static HttpResponse text(string data);
       void setHeader(const string& key, cpnst string& val);
       string toString() const;

    private:
        int statusCode;
        map<string, string> headers;
        string body;
};

#endif