#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <map>
#include <string>
using namespace std;

// A structured HTTP response, convertible to raw wire bytes
//
// Holds: status code, headers, body. Provides helpers like HttpResponse::json(...) that generate a full HTTP response with correct Content-Type and Content-Length.
class HttpResponse {
    public:
        /*
            Builds a 200 response with Content-Type: application/json and the given body, automatically
            computing Content-Length.
        */
       static HttpResponse json(const std::string& data);

       /*
        Builds a plain-text response with Content-Type: text/plain, again setting Content-Length
        automatically.
       */
       static HttpResponse text(const std::string& data);

       // Sets or overwrites a single response header
       void setHeader(const std::string& key, const std::string& value);

       /*
        Serializes the full response (status line, headers, blank line, body) into the exact bytes to write t the socket.

        Expected Output:
            HTTP/1.1 200 OK
            Content-Type: application/json
            Content-Length: 27

            {"message": "Hello World"}
       */
       std::string toString() const;

    private:
        int statusCode;
        map<std::string, std::string> headers;
        std::string body;
};

#endif