#ifndef CONFIGURATIONCONTROLLER_H
#define CONFIGURATIONCONTROLLER_H

#include "HTTP/HttpRequest.h"
#include "HTTP/HttpResponse.h"
#include "Server/ServerConfig.h"

#include <string>

class ConfigurationController {
    public:
        HttpResponse getConfig(const ServerConfig& config);

        bool parseConfig(
            const HttpRequest& request,
            ServerConfig& config,
            std::string& error
        );
};

#endif