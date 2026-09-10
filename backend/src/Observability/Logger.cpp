#include "Observability/Logger.h"

#include <iostream>

void Logger::onEvent(const ServerEvent& e) {
    switch (e.type) {
        case ServerEventType::ServerStarted:
            std::cout << "Server started\n";
            break;

        case ServerEventType::ServerStopped:
            std::cout << "Server stopped\n";
            break;

        case ServerEventType::RequestReceived:
            std::cout << "Request received: "
                      << e.method << " " << e.path << '\n';
            break;

        case ServerEventType::ResponseSent:
            std::cout << "Response sent: "
                      << e.statusCode << " "
                      << e.durationMs << "ms\n";
            break;
    }
}