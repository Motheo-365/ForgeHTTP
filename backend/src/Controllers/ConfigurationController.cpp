#include "Controllers/ConfigurationController.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

HttpResponse ConfigurationController::getConfig (const ServerConfig& config) {
    json response = {
        {"port", config.port},
        {"workerThreads", config.workerThreads},
        {"rateLimit", config.rateLimit},
        {"requestHistoryLimit", config.requestHistoryLimit},
        {"corsEnabled", config.corsEnabled},
        {"authenticationEnabled", config.authenticationEnabled},
        {"loggingEnabled", config.loggingEnabled}
    };

    return HttpResponse::json(response.dump());
}

bool ConfigurationController::parseConfig(
    const HttpRequest& request,
    ServerConfig& config,
    std::string& error
) {
    try {
        const json body = json::parse(request.getBody());

        ServerConfig updated = config;

        if (body.contains("port")) {
            const int port = body.at("port").get<int>();

            if (port < 1 || port > 65535) {
                error = "Invalid port";
                return false;
            }

            updated.port = port;
        }

        if (body.contains("workerThreads")) {
            const std::size_t workers =
                body.at("workerThreads").get<std::size_t>();

            if (workers < 1 || workers > 64) {
                error = "Worker threads must be between 1 and 64";
                return false;
            }

            updated.workerThreads = workers;
        }

        if (body.contains("rateLimit")) {
            const std::size_t rateLimit =
                body.at("rateLimit").get<std::size_t>();

            if (rateLimit < 1 || rateLimit > 10000) {
                error = "Rate limit must be between 1 and 10000";
                return false;
            }

            updated.rateLimit = rateLimit;
        }

        if (body.contains("requestHistoryLimit")) {
            const std::size_t historyLimit =
                body.at("requestHistoryLimit").get<std::size_t>();

            if (historyLimit < 1 || historyLimit > 10000) {
                error = "Request history limit must be between 1 and 10000";
                return false;
            }

            updated.requestHistoryLimit = historyLimit;
        }

        if (body.contains("corsEnabled")) {
            updated.corsEnabled =
                body.at("corsEnabled").get<bool>();
        }

        if (body.contains("authenticationEnabled")) {
            updated.authenticationEnabled =
                body.at("authenticationEnabled").get<bool>();
        }

        if (body.contains("loggingEnabled")) {
            updated.loggingEnabled =
                body.at("loggingEnabled").get<bool>();
        }

        config = updated;
        return true;
    }
    catch (const json::exception&) {
        error = "Invalid configuration JSON";
        return false;
    }
}