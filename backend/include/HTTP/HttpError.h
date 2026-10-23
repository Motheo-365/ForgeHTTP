#ifndef HTTPERROR_H
#define HTTPERROR_H

#include <stdexcept>
#include <string>

class HttpError : public std::runtime_error {
public:
    HttpError(int statusCode, const std::string& message)
        : std::runtime_error(message), statusCode(statusCode) {}

    int getStatusCode() const noexcept {
        return statusCode;
    }

private:
    int statusCode;
};

#endif
