#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include <cstddef>

struct ServerConfig {
    int port = 8086;
    std::size_t workerThreads = 8;
    std::size_t rateLimit = 100;
    std::size_t requestHistoryLimit = 1000;
    bool corsEnabled = true;
    bool authenticationEnabled = true;
    bool loggingEnabled = true;
};

#endif