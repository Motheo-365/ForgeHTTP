#include "HTTP/HttpResponse.h"
#include <sstream>

HttpResponse HttpResponse::json(const std::string& data) {
    HttpResponse response;

    response.statusCode = 200;
    response.headers["Content-Type"] = "application/json";
    response.headers["Content-Length"] = std::to_string(data.size());
    response.body = data;

    return response;
}

HttpResponse HttpResponse::text(const std::string& data) {
    HttpResponse response;

    response.statusCode = 200;
    response.headers["Content-Type"] = "text/plain";
    response.headers["Content-Length"] = std::to_string(data.size());
    response.body = data;

    return response;
}

void HttpResponse::setHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}

void HttpResponse::setStatusCode(int code) {
    statusCode = code;
}


std::string HttpResponse::toString() const {
    std::ostringstream buffer;
    std::string statusMessage = "OK";

    if (statusCode == 201) {
            statusMessage = "Created";
    }
    
    else if (statusCode == 400) {
        statusMessage = "Bad Request";
    }

    else if (statusCode == 404) {
        statusMessage = "Not Found";
    }

    else if (statusCode == 500) {
        statusMessage = "Internal Server Error";
    }

    buffer << "HTTP/1.1 "
           << statusCode
           << " "
           << statusMessage
           << "\r\n";

    for (const auto& header : headers) {
        buffer << header.first << ": " << header.second << "\r\n";
    }

    buffer << "\r\n";
    buffer << body;

    return buffer.str();
}