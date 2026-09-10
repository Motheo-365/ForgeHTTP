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
    ServerEventType type = ServerEventType::RequestReceived;

    std::chrono::system_clock::time_point timestamp;

    std::string method;
    std::string path;

    int statusCode = 0;

    double durationMs = 0.0;
};

#endif