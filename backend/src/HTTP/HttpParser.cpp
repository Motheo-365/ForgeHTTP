#include "HTTP/HttpParser.h"
#include "HTTP/HttpError.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace {
constexpr std::size_t MaxRequestLineBytes = 8192;
constexpr std::size_t MaxHeaderBytes = 32768;
constexpr std::size_t MaxBodyBytes = 1024 * 1024;

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

bool isTokenCharacter(unsigned char c) {
    return std::isalnum(c) || std::string("!#$%&'*+-.^_`|~").find(c) != std::string::npos;
}

std::string decodeQueryComponent(const std::string& input) {
    std::string decoded;
    for (std::size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '+') {
            decoded.push_back(' ');
        } else if (input[i] == '%') {
            if (i + 2 >= input.size()) {
                throw HttpError(400, "Invalid URL encoding in query string");
            }
            const auto hexValue = [](unsigned char c) -> int {
                if (c >= '0' && c <= '9') return c - '0';
                if (c >= 'a' && c <= 'f') return c - 'a' + 10;
                if (c >= 'A' && c <= 'F') return c - 'A' + 10;
                return -1;
            };
            const int high = hexValue(static_cast<unsigned char>(input[i + 1]));
            const int low = hexValue(static_cast<unsigned char>(input[i + 2]));
            if (high < 0 || low < 0) {
                throw HttpError(400, "Invalid URL encoding in query string");
            }
            decoded.push_back(static_cast<char>((high << 4) | low));
            i += 2;
        } else {
            decoded.push_back(input[i]);
        }
    }
    return decoded;
}
}

HttpRequest HttpParser::parse(const std::string& raw) {
    HttpRequest request;
    const std::size_t headerEnd = raw.find("\r\n\r\n");
    const std::size_t delimiterSize = headerEnd == std::string::npos ? 2 : 4;
    const std::size_t alternateEnd = headerEnd == std::string::npos ? raw.find("\n\n") : headerEnd;
    const std::size_t actualHeaderEnd = alternateEnd;
    if (actualHeaderEnd == std::string::npos) {
        throw HttpError(400, "Incomplete HTTP headers");
    }
    if (actualHeaderEnd > MaxHeaderBytes) {
        throw HttpError(413, "HTTP headers exceed the configured limit");
    }

    const std::string headerBlock = raw.substr(0, actualHeaderEnd);
    const std::size_t requestLineEnd = headerBlock.find('\n');
    std::string requestLine = headerBlock.substr(0, requestLineEnd);
    if (!requestLine.empty() && requestLine.back() == '\r') {
        requestLine.pop_back();
    }
    if (requestLine.empty() || requestLine.size() > MaxRequestLineBytes) {
        throw HttpError(400, "Invalid request line");
    }
    std::istringstream line(requestLine);

    std::string method;
    std::string path;
    std::string version;
    std::string extra;
    if (!(line >> method >> path >> version) || (line >> extra)) {
        throw HttpError(400, "Malformed request line");
    }

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
        throw HttpError(501, "Unsupported HTTP method");
    }

    if (version != "HTTP/1.1") {
        throw HttpError(505, "HTTP version is not supported");
    }
    if (path.empty() || path.front() != '/' ||
        std::any_of(path.begin(), path.end(), [](unsigned char c) {
            return c <= 0x20 || c == 0x7f;
        })) {
        throw HttpError(400, "Invalid request target");
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

    const std::size_t firstHeader = requestLineEnd == std::string::npos
        ? headerBlock.size() : requestLineEnd + 1;
    request.headers = parseHeaders(headerBlock.substr(firstHeader));
    if (request.getHeader("Host").empty()) {
        throw HttpError(400, "Missing Host header");
    }

    std::size_t contentLength = 0;
    bool hasContentLength = false;
    bool hasTransferEncoding = false;
    for (const auto& header : request.headers) {
        const std::string name = lower(header.first);
        if (name == "content-length") {
            if (hasContentLength) {
                throw HttpError(400, "Duplicate Content-Length header");
            }
            hasContentLength = true;
            if (header.second.empty() ||
                !std::all_of(header.second.begin(), header.second.end(), [](unsigned char c) {
                    return c >= '0' && c <= '9';
                })) {
                throw HttpError(400, "Invalid Content-Length");
            }
            try {
                std::size_t parsed = 0;
                contentLength = std::stoull(header.second, &parsed);
                if (parsed != header.second.size()) {
                    throw HttpError(400, "Invalid Content-Length");
                }
            } catch (const std::invalid_argument&) {
                throw HttpError(400, "Invalid Content-Length");
            } catch (const std::out_of_range&) {
                throw HttpError(413, "Request body exceeds the configured limit");
            }
        } else if (name == "transfer-encoding") {
            hasTransferEncoding = true;
        }
    }
    if (hasTransferEncoding && hasContentLength) {
        throw HttpError(400, "Content-Length and Transfer-Encoding cannot be combined");
    }
    if (hasTransferEncoding) {
        throw HttpError(501, "Transfer-Encoding is not supported");
    }
    if (contentLength > MaxBodyBytes) {
        throw HttpError(413, "Request body exceeds the configured limit");
    }

    const std::size_t bodyStart = actualHeaderEnd + delimiterSize;
    const std::size_t availableBody = raw.size() - bodyStart;
    if (availableBody < contentLength) {
        throw HttpError(400, "Request body length does not match Content-Length");
    }
    request.body = raw.substr(bodyStart, contentLength);
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

        const std::size_t colon = line.find(':');
        if (colon == std::string::npos || colon == 0) {
            throw HttpError(400, "Malformed HTTP header");
        }

        std::string key = lower(line.substr(0, colon));
        if (!std::all_of(key.begin(), key.end(), [](unsigned char c) {
                return isTokenCharacter(c);
            })) {
            throw HttpError(400, "Invalid HTTP header name");
        }
        std::string value = line.substr(colon + 1);
        if (std::any_of(value.begin(), value.end(), [](unsigned char c) {
                return (c < 0x20 && c != '\t') || c == 0x7f;
            })) {
            throw HttpError(400, "Invalid HTTP header value");
        }

        std::size_t start = value.find_first_not_of(" \t");

        if (start != std::string::npos) {
            value = value.substr(start);
        }

        else {
            value = "";
        }
        const std::size_t last = value.find_last_not_of(" \t");
        if (last != std::string::npos) {
            value.erase(last + 1);
        }

        const auto existing = resultMap.find(key);
        if (existing != resultMap.end()) {
            if (key == "content-length" || key == "host") {
                throw HttpError(400, "Duplicate HTTP header");
            }
            existing->second += ", " + value;
        } else {
            resultMap.emplace(key, value);
        }
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

        std::string key = decodeQueryComponent(parameter.substr(0, equals));
        std::string value = decodeQueryComponent(parameter.substr(equals + 1));

        resultMap[key] = value;
    }

    return resultMap;
}