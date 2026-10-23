#include "HTTP/HttpResponse.h"
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>

namespace {
std::string lowerHeaderName(std::string name) {
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return name;
}

bool validHeaderName(const std::string& name) {
    if (name.empty()) return false;
    return std::all_of(name.begin(), name.end(), [](unsigned char c) {
        return std::isalnum(c) ||
            std::string("!#$%&'*+-.^_`|~").find(c) != std::string::npos;
    });
}
}

HttpResponse HttpResponse::json(const std::string& data) {
    HttpResponse response;

    response.statusCode = 200;
    response.headers["Content-Type"] = "application/json";
    response.headers["Content-Length"] = std::to_string(data.size());
    response.headers["Connection"] = "close";
    response.body = data;

    return response;
}

HttpResponse HttpResponse::text(const std::string& data) {
    HttpResponse response;

    response.statusCode = 200;
    response.headers["Content-Type"] = "text/plain";
    response.headers["Content-Length"] = std::to_string(data.size());
    response.headers["Connection"] = "close";
    response.body = data;

    return response;
}

void HttpResponse::setHeader(const std::string& key, const std::string& value) {
    if (!validHeaderName(key) ||
        std::any_of(value.begin(), value.end(), [](unsigned char c) {
            return (c < 0x20 && c != '\t') || c == 0x7f;
        })) {
        throw std::invalid_argument("Invalid HTTP response header");
    }
    const std::string normalized = lowerHeaderName(key);
    const auto existing = std::find_if(headers.begin(), headers.end(),
        [&normalized](const auto& header) {
            return lowerHeaderName(header.first) == normalized;
        });
    if (existing == headers.end()) {
        headers[key] = value;
    } else {
        existing->second = value;
    }
}

void HttpResponse::setStatusCode(const int& code) {
    if (code < 100 || code > 599) {
        throw std::invalid_argument("HTTP status code must be between 100 and 599");
    }
    statusCode = code;
}

void HttpResponse::setBody(const std::string& data) {
    body = data;
    headers["Content-Length"] = std::to_string(data.size());
}

std::string HttpResponse::toString() const {
    std::ostringstream buffer;
    static const std::map<int, std::string> reasonPhrases = {
        {200, "OK"}, {201, "Created"}, {204, "No Content"},
        {400, "Bad Request"}, {401, "Unauthorized"},
        {403, "Forbidden"}, {404, "Not Found"},
        {405, "Method Not Allowed"}, {408, "Request Timeout"},
        {413, "Payload Too Large"}, {415, "Unsupported Media Type"},
        {429, "Too Many Requests"}, {500, "Internal Server Error"},
        {501, "Not Implemented"}, {503, "Service Unavailable"},
        {505, "HTTP Version Not Supported"}
    };
    const auto phrase = reasonPhrases.find(statusCode);
    const std::string statusMessage =
        phrase == reasonPhrases.end() ? "Unknown" : phrase->second;

    std::map<std::string, std::string> responseHeaders = headers;
    responseHeaders["Connection"] = "close";
    if (statusCode == 204) {
        responseHeaders.erase("Content-Length");
    } else {
        responseHeaders["Content-Length"] = std::to_string(body.size());
    }
    buffer << "HTTP/1.1 "
           << statusCode
           << " "
           << statusMessage
           << "\r\n";

    for (const auto& header : responseHeaders) {
        buffer << header.first << ": " << header.second << "\r\n";
    }

    buffer << "\r\n";
    if (statusCode != 204) {
        buffer << body;
    }

    return buffer.str();
}

int HttpResponse::getStatusCode() const {
    return statusCode;
}