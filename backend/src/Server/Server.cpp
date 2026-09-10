#include "Server/Server.h"

#include "HTTP/HttpParser.h"
#include "HTTP/HttpResponse.h"
#include "HTTP/HttpRequest.h"
#include "Networking/Connection.h"

#include <cstdlib>
#include <iostream>
#include <memory>

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

Server::Server()
    : config(),
      pool(config.workerThreads),
      metricsConnector(config.workerThreads),
      rateLimiterMiddleware(config.rateLimit),
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

    router.get("/api/requests", [this](const HttpRequest& req) {
        return requestHistory.getRequests();
    });

    router.post("/api/users", [this](const HttpRequest& req) {
        return userController.createUser(req);
    });

    router.get("/metrics", [this](const HttpRequest&) {
        return metricsConnector.getMetrics();
    });

    router.get("/api/config", [this](const HttpRequest&) {
        return configurationController.getConfig(config);
    });

    router.put("/api/config", [this](const HttpRequest& request) {
        std::string error;

        if (!configurationController.parseConfig(request, config, error)) {
            HttpResponse response = HttpResponse::json(
                "{\"error\":\"" + error + "\"}"
            );
            response.setStatusCode(400);
            return response;
        }

        updateConfig(config);
        return configurationController.getConfig(config);
    });
}

Server::~Server() {
    stop();
}

void Server::start(int port) {
    if (port == 0) {
        const char* environmentPort = std::getenv("PORT");

        if (environmentPort != nullptr) {
            port = std::stoi(environmentPort);
        }
        else {
            port = config.port;
        }
    }

    listenSocket.bind(port);
    listenSocket.listen(10);

    running = true;

    std::cout << "ForgeHTTP\n"
              << "----------------------------------------\n";

    std::cout << "\nServer running on port "
              << port << '\n';

    std::cout << "Workers: "
              << config.workerThreads
              << "\n\n";

    events.publish({
        ServerEventType::ServerStarted,
        std::chrono::system_clock::now()
    });

    acceptConnections();
}

void Server::stop() {
    const bool wasRunning = running;
    running = false;
    listenSocket.close();
    pool.shutdown();

    if (wasRunning) {
        events.publish({ServerEventType::ServerStopped, std::chrono::system_clock::now()});
    }
}

void Server::acceptConnections() {
    while (running) {
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

        pool.enqueue(
            [this, connection]() {
                handleConnection(*connection);
            }
        );
    }
}

void Server::handleConnection(Connection& c) {
    const auto requestStarted = std::chrono::steady_clock::now();

    try {
        std::string rawRequest = c.read();

        // std::cout << "Received request:\n";
        // std::cout << rawRequest << '\n';

        HttpRequest request = HttpParser::parse(rawRequest);
        const bool internalRequest =
            request.getMethod() == HttpMethod::OPTIONS ||
            request.getHeader("X-ForgeHTTP-Internal") == "true";

        events.publish({
            ServerEventType::RequestReceived,
            std::chrono::system_clock::now(),
            methodName(request.getMethod()),
            request.getPath(),
            0,
            0.0,
            internalRequest
        });

        std::cout << "Parsed path: "
                  << request.getPath()
                  << '\n';

        HttpResponse response;
        auto endpoint = [&]() {
            Route* route = router.match(request);

            if (route == nullptr) {
                response = HttpResponse::text("Route not found");
                response.setStatusCode(404);
                return;
            }

            response = route->getHandler()(request);
        };

        corsMiddleware.handle(request, response, [&]() {
            loggerMiddleware.handle(request, response, [&]() {
                rateLimiterMiddleware.handle(request, response, [&]() {
                    if (request.getPath().rfind("/api/", 0) == 0) {
                        authMiddleware.handle(request, response, endpoint);
                    }
                    else {
                        endpoint();
                    }
                });
            });
        });

        c.write(response.toString());

        const auto elapsed = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - requestStarted).count();
        events.publish({
            ServerEventType::ResponseSent,
            std::chrono::system_clock::now(),
            methodName(request.getMethod()),
            request.getPath(),
            response.getStatusCode(),
            elapsed,
            internalRequest
        });
    }

    catch (const std::exception& e) {
        std::cerr << "Request error: "
                  << e.what()
                  << '\n';

        HttpResponse response =
            HttpResponse::text("Bad Request");

        response.setStatusCode(400);

        c.write(response.toString());
    }

    c.close();
}

const ServerConfig& Server::getConfig() const {
    return config;
}

void Server::updateConfig(const ServerConfig& newConfig) {
    config = newConfig;

    rateLimiterMiddleware.setLimit(
        config.rateLimit
    );

    requestHistory.setMaxEntries(
        config.requestHistoryLimit
    );
}