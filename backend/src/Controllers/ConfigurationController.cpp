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
        if (!body.is_object()) {
            error = "Configuration must be a JSON object";
            return false;
        }

        ServerConfig updated = config;
        for (auto it = body.begin(); it != body.end(); ++it) {
            if (it.key() != "port" && it.key() != "workerThreads" &&
                it.key() != "rateLimit" && it.key() != "requestHistoryLimit" &&
                it.key() != "corsEnabled" && it.key() != "authenticationEnabled" &&
                it.key() != "loggingEnabled") {
                error = "Unknown configuration option: " + it.key();
                return false;
            }
        }

        if (body.contains("port")) {
            if (!body.at("port").is_number_integer()) {
                error = "Port must be an integer";
                return false;
            }
            const int port = body.at("port").get<int>();

            if (port < 1 || port > 65535) {
                error = "Invalid port";
                return false;
            }
            if (port != config.port) {
                error = "Changing port requires a server restart";
                return false;
            }

            updated.port = port;
        }

        if (body.contains("workerThreads")) {
            if (!body.at("workerThreads").is_number_integer()) {
                error = "workerThreads must be a positive integer";
                return false;
            }
            const std::size_t workers =
                body.at("workerThreads").get<std::size_t>();

            if (workers < 1 || workers > 64) {
                error = "Worker threads must be between 1 and 64";
                return false;
            }
            if (workers != config.workerThreads) {
                error = "Changing workerThreads requires a server restart";
                return false;
            }

            updated.workerThreads = workers;
        }

        if (body.contains("rateLimit")) {
            if (!body.at("rateLimit").is_number_integer()) {
                error = "rateLimit must be a positive integer";
                return false;
            }
            const std::size_t rateLimit =
                body.at("rateLimit").get<std::size_t>();

            if (rateLimit < 1 || rateLimit > 10000) {
                error = "Rate limit must be between 1 and 10000";
                return false;
            }

            updated.rateLimit = rateLimit;
        }

        if (body.contains("requestHistoryLimit")) {
            if (!body.at("requestHistoryLimit").is_number_integer()) {
                error = "requestHistoryLimit must be a positive integer";
                return false;
            }
            const std::size_t historyLimit =
                body.at("requestHistoryLimit").get<std::size_t>();

            if (historyLimit < 1 || historyLimit > 10000) {
                error = "Request history limit must be between 1 and 10000";
                return false;
            }

            updated.requestHistoryLimit = historyLimit;
        }

        if (body.contains("corsEnabled")) {
            if (!body.at("corsEnabled").is_boolean()) {
                error = "corsEnabled must be a boolean";
                return false;
            }
            updated.corsEnabled =
                body.at("corsEnabled").get<bool>();
        }

        if (body.contains("authenticationEnabled")) {
            if (!body.at("authenticationEnabled").is_boolean()) {
                error = "authenticationEnabled must be a boolean";
                return false;
            }
            updated.authenticationEnabled =
                body.at("authenticationEnabled").get<bool>();
        }

        if (body.contains("loggingEnabled")) {
            if (!body.at("loggingEnabled").is_boolean()) {
                error = "loggingEnabled must be a boolean";
                return false;
            }
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