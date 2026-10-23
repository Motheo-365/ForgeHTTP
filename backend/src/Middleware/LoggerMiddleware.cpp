#include "Middleware/LoggerMiddleware.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

namespace {
std::string methodName(HttpMethod method) {
    switch (method) {
        case HttpMethod::GET: return "GET";
        case HttpMethod::POST: return "POST";
        case HttpMethod::PUT: return "PUT";
        case HttpMethod::DELETE: return "DELETE";
        case HttpMethod::PATCH: return "PATCH";
        case HttpMethod::OPTIONS: return "OPTIONS";
    }

    return "UNKNOWN";
}
}

void LoggerMiddleware::handle(HttpRequest& req, HttpResponse& res, std::function<void()> next) {
    const auto start = std::chrono::steady_clock::now();
    next();
    if (!enabled.load()) {
        return;
    }

    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();
    const std::time_t now = std::time(nullptr);
    std::tm local_time{};
    localtime_r(&now, &local_time);

    std::cout << '[' << std::put_time(&local_time, "%H:%M:%S") << "] "
              << methodName(req.getMethod()) << ' '
              << std::string(req.getPath()) << ' '
              << res.getStatusCode() << ' ' << elapsed << "ms\n";
}

void LoggerMiddleware::setEnabled(bool value) {
    enabled.store(value);
}