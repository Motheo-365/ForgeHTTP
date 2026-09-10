#include "HTTP/HttpParser.h"

#include <sstream>
#include <stdexcept>

HttpRequest HttpParser::parse(const std::string& raw) {
    HttpRequest request;
    std::istringstream stream(raw);

    // Read request line
    std::string requestLine;
    std::getline(stream, requestLine);
    std::istringstream line(requestLine);

    std::string method;
    std::string path;
    std::string version;

    line >> method >> path >> version;

    // Parse HTTP method
    if (method == "GET") {
        request.method = HttpMethod::GET;
    }

    else if (method == "POST") {
        request.method = HttpMethod::POST;
    }

    else if (method == "PUT") {
        request.method = HttpMethod::PUT;
    }

    else if (method == "DELETE") {
        request.method = HttpMethod::DELETE;
    }

    else if (method == "PATCH") {
        request.method = HttpMethod::PATCH;
    }

    else if (method == "OPTIONS") {
        request.method = HttpMethod::OPTIONS;
    }

    else {
        throw std::runtime_error("Unsupported HTTP method");
    }

    request.path = path;
    request.version = version;

    // Parse query parameters
    request.query = parseQueryString(path);

    // Remove query string from stored path
    std::size_t questionMark = path.find('?');

    if (questionMark != std::string::npos) {
        request.path = path.substr(0, questionMark);
    }

    // Read headers
    std::string headerData;
    std::string currentLine;

    while (std::getline(stream, currentLine)) {
        // Remove the '\r' from CRLF
        if (!currentLine.empty() && currentLine.back() == '\r') {
            currentLine.pop_back();
        }

        // Empty line means headers are finished
        if (currentLine.empty()) {
            break;
        }

        headerData += currentLine;
        headerData += '\n';
    }

    request.headers = parseHeaders(headerData);
    request.clientAddress = request.getHeader("X-Forwarded-For");

    if (request.clientAddress.empty()) {
        request.clientAddress = request.getHeader("Host");
    }

    // Read body
    std::string body;

    while (std::getline(stream, currentLine)) {
        body += currentLine;

        if (!stream.eof()) {
            body += '\n';
        }
    }

    request.body = body;
    return request;
}

std::map<std::string, std::string> HttpParser::parseHeaders(const std::string& raw) {
    std::map<std::string, std::string> resultMap;

    std::istringstream stream(raw);
    std::string line;

    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        std::size_t colon = line.find(':');

        if (colon == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 1);

        std::size_t start = value.find_first_not_of(" \t");

        if (start != std::string::npos) {
            value = value.substr(start);
        }

        else {
            value = "";
        }

        resultMap[key] = value;
    }

    return resultMap;
}

std::map<std::string, std::string> HttpParser::parseQueryString(const std::string& path) {
    std::map<std::string, std::string> resultMap;
    std::size_t questionMark = path.find('?');

    if (questionMark == std::string::npos) {
        return resultMap;
    }

    std::string queryString = path.substr(questionMark + 1);

    std::istringstream stream(queryString);
    std::string parameter;

    while (std::getline(stream, parameter, '&')) {
        std::size_t equals = parameter.find('=');

        if (equals == std::string::npos) {
            continue;
        }

        std::string key = parameter.substr(0, equals);
        std::string value = parameter.substr(equals + 1);

        resultMap[key] = value;
    }

    return resultMap;
}