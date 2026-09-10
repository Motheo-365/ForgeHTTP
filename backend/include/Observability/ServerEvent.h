#ifndef SERVEREVENT_H
#define SERVEREVENT_H

#include <chrono>
#include <string>

enum class ServerEventType {
    RequestReceived,
    ResponseSent,
    ServerStarted,
    ServerStopped
};

struct ServerEvent {
    ServerEventType type;

    std::chrono::system_clock::time_point timestamp;

    std::string method;
    std::string path;

    int statusCode;

    double durationMs;
};

#endif