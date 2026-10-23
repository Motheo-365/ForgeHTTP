#include "Server/Server.h"

#include "HTTP/HttpParser.h"
#include "HTTP/HttpResponse.h"
#include "HTTP/HttpRequest.h"
#include "Networking/Connection.h"
#include "HTTP/HttpError.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <csignal>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace {
volatile std::sig_atomic_t stopSignalReceived = 0;

void handleStopSignal(int) {
    stopSignalReceived = 1;
}

std::string configuredApiToken() {
    const char* token = std::getenv("FORGEHTTP_API_TOKEN");
    return token == nullptr ? "" : token;
}

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

Server::Server()
    : config(),
      pool(config.workerThreads),
      authMiddleware(configuredApiToken()),
      rateLimiterMiddleware(config.rateLimit),
      metricsConnector(config.workerThreads),
      requestHistory(config.requestHistoryLimit) {
    events.subscribe(&logger);
    events.subscribe(&metricsConnector);
    events.subscribe(&requestHistory);

    router.get("/health", [this](const HttpRequest& req) {
        return healthController.getHealth(req);
    });

    router.get("/api/users", [this](const HttpRequest& req) {
        return userController.getUsers(req);
    });

    router.get("/api/requests", [this](const HttpRequest&) {
        return requestHistory.getRequests();
    });

    router.post("/api/users", [this](const HttpRequest& req) {
        return userController.createUser(req);
    });
    router.get("/api/users/:id", [this](const HttpRequest& req) {
        return userController.getUser(req);
    });
    router.put("/api/users/:id", [this](const HttpRequest& req) {
        return userController.updateUser(req);
    });
    router.del("/api/users/:id", [this](const HttpRequest& req) {
        return userController.deleteUser(req);
    });

    router.get("/metrics", [this](const HttpRequest&) {
        return metricsConnector.getMetrics();
    });

    router.get("/api/config", [this](const HttpRequest&) {
        return configurationController.getConfig(getConfig());
    });

    router.put("/api/config", [this](const HttpRequest& request) {
        std::string error;
        ServerConfig updated = getConfig();

        if (!configurationController.parseConfig(request, updated, error)) {
            HttpResponse response = HttpResponse::json(nlohmann::json{{"error", error}}.dump());
            response.setStatusCode(400);
            return response;
        }

        try {
            updateConfig(updated);
        } catch (const std::exception& e) {
            HttpResponse response = HttpResponse::json(
                nlohmann::json{{"error", e.what()}}.dump());
            response.setStatusCode(400);
            return response;
        }
        return configurationController.getConfig(getConfig());
    });
}

Server::~Server() {
    stop();
}

void Server::start(int port) {
    const ServerConfig startupConfig = getConfig();
    if (port == 0) {
        const char* environmentPort = std::getenv("PORT");

        if (environmentPort != nullptr) {
            port = std::stoi(environmentPort);
        }
        else {
            port = startupConfig.port;
        }
    }

    listenSocket.bind(port);
    listenSocket.listen(128);
    {
        std::lock_guard<std::mutex> lock(configMutex);
        config.port = port;
    }

    stopSignalReceived = 0;
    std::signal(SIGINT, handleStopSignal);
    std::signal(SIGTERM, handleStopSignal);
    acceptLoopActive = true;
    running = true;

    std::cout << "ForgeHTTP\n"
              << "----------------------------------------\n";

    std::cout << "\nServer running on port "
              << port << '\n';

    std::cout << "Workers: "
              << getConfig().workerThreads
              << "\n\n";

    events.publish({
        ServerEventType::ServerStarted,
        std::chrono::system_clock::now(),
        {},
        {},
        0,
        0.0
    });

    acceptConnections();
    listenSocket.close();
    acceptLoopActive = false;
    stop();
}

void Server::stop() {
    const bool wasRunning = running.exchange(false);
    if (!acceptLoopActive.load()) {
        listenSocket.close();
    }
    pool.shutdown();

    if (wasRunning) {
        events.publish({
            ServerEventType::ServerStopped,
            std::chrono::system_clock::now(),
            {},
            {},
            0,
            0.0
        });
    }
}

void Server::acceptConnections() {
    while (running) {
        if (stopSignalReceived != 0) {
            break;
        }
        try {
            if (!listenSocket.waitForConnection(500)) {
                continue;
            }
        } catch (const std::exception&) {
            if (!running) {
                break;
            }
            throw;
        }
        if (!running) {
            break;
        }

        Socket client;

        try {
            client = listenSocket.accept();
        }
        catch (const std::exception&) {
            if (!running) {
                break;
            }
            throw;
        }

        auto connection = std::make_shared<Connection>(std::move(client));

        const bool queued = pool.enqueue(
            [this, connection]() {
                handleConnection(*connection);
            }
        );
        if (!queued) {
            try {
                HttpResponse response = HttpResponse::json(
                    "{\"error\":\"Server is overloaded\"}");
                response.setStatusCode(503);
                connection->write(response.toString());
            } catch (const std::exception& e) {
                std::cerr << "Unable to send overload response: " << e.what() << '\n';
            }
            connection->close();
        }
    }
}

void Server::handleConnection(Connection& c) {
    const auto requestStarted = std::chrono::steady_clock::now();
    bool requestTracked = false;
    bool responseWriteStarted = false;
    std::string requestMethod;
    std::string requestPath;

    try {
        std::string rawRequest = c.read();

        // std::cout << "Received request:\n";
        // std::cout << rawRequest << '\n';

        HttpRequest request = HttpParser::parse(rawRequest);
        request.clientAddress = c.peerAddress();
        requestMethod = methodName(request.getMethod());
        requestPath = request.getPath();

        events.publish({
            ServerEventType::RequestReceived,
            std::chrono::system_clock::now(),
            requestMethod,
            requestPath
        });
        requestTracked = true;

        HttpResponse response;
        auto endpoint = [&]() {
            Route* route = router.match(request);

            if (route == nullptr) {
                if (router.hasPath(request.getPath())) {
                    response = HttpResponse::text("Method not allowed");
                    response.setStatusCode(405);
                    response.setHeader("Allow", router.allowedMethods(request.getPath()));
                } else {
                    response = HttpResponse::json("{\"error\":\"Not Found\"}");
                    response.setStatusCode(404);
                }
                return;
            }

            try {
                response = route->getHandler()(request);
            } catch (const std::exception& e) {
                std::cerr << "Route handler error: " << e.what() << '\n';
                response = HttpResponse::json("{\"error\":\"Internal Server Error\"}");
                response.setStatusCode(500);
            }
        };

        auto process = [&]() {
            rateLimiterMiddleware.handle(request, response, [&]() {
                if (authenticationEnabled.load() &&
                    request.getPath().rfind("/api/", 0) == 0) {
                    authMiddleware.handle(request, response, endpoint);
                } else {
                    endpoint();
                }
            });
        };
        loggerMiddleware.handle(request, response, [&]() {
            if (corsEnabled.load()) {
                corsMiddleware.handle(request, response, process);
            } else {
                process();
            }
        });

        const std::string serializedResponse = response.toString();
        responseWriteStarted = true;
        c.write(serializedResponse);

        const auto elapsed = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - requestStarted).count();
        events.publish({
            ServerEventType::ResponseSent,
            std::chrono::system_clock::now(),
            methodName(request.getMethod()),
            request.getPath(),
            response.getStatusCode(),
            elapsed
        });
    }

    catch (const HttpError& e) {
        if (requestTracked) {
            events.publish({
                ServerEventType::RequestFailed,
                std::chrono::system_clock::now(),
                requestMethod,
                requestPath,
                e.getStatusCode(),
                std::chrono::duration<double, std::milli>(
                    std::chrono::steady_clock::now() - requestStarted).count()
            });
        }
        if (!responseWriteStarted) {
            HttpResponse response = HttpResponse::json(
                nlohmann::json{{"error", e.what()}}.dump());
            response.setStatusCode(e.getStatusCode());
            try {
                c.write(response.toString());
            } catch (const std::exception& writeError) {
                std::cerr << "Unable to send HTTP error response: "
                          << writeError.what() << '\n';
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Request error: "
                  << e.what()
                  << '\n';
        if (requestTracked) {
            events.publish({
                ServerEventType::RequestFailed,
                std::chrono::system_clock::now(),
                requestMethod,
                requestPath,
                500,
                std::chrono::duration<double, std::milli>(
                    std::chrono::steady_clock::now() - requestStarted).count()
            });
        }

        if (!responseWriteStarted) {
            HttpResponse response = HttpResponse::json(
                "{\"error\":\"Internal Server Error\"}");
            response.setStatusCode(500);
            try {
                c.write(response.toString());
            } catch (const std::exception& writeError) {
                std::cerr << "Unable to send server error response: "
                          << writeError.what() << '\n';
            }
        }
    }

    c.close();
}

ServerConfig Server::getConfig() const {
    std::lock_guard<std::mutex> lock(configMutex);
    return config;
}

void Server::updateConfig(const ServerConfig& newConfig) {
    std::lock_guard<std::mutex> lock(configMutex);
    if (newConfig.port != config.port || newConfig.workerThreads != config.workerThreads) {
        throw std::invalid_argument(
            "port and workerThreads require a server restart");
    }
    config = newConfig;

    rateLimiterMiddleware.setLimit(
        config.rateLimit
    );

    requestHistory.setMaxEntries(
        config.requestHistoryLimit
    );
    corsEnabled.store(config.corsEnabled);
    authenticationEnabled.store(config.authenticationEnabled);
    loggerMiddleware.setEnabled(config.loggingEnabled);
}