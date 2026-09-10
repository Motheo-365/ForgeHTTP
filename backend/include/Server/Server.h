#ifndef SERVER_H
#define SERVER_H

#include "Networking/Socket.h"
#include "Networking/Connection.h"
#include "Concurrency/ThreadPool.h"
#include "Routing/Router.h"
#include "Controllers/HealthController.h"
#include "Controllers/UserController.h"
#include "Middleware/CorsMiddleware.h"
#include "Middleware/AuthMiddleware.h"
#include "Middleware/LoggerMiddleware.h"
#include "Middleware/RateLimiterMiddleware.h"
#include "Observability/Logger.h"
#include "Observability/MetricsConnector.h"
#include "Observability/ServerEventPublisher.h"
#include "Observability/RequestHistory.h"
#include "ServerConfig.h"
#include "Controllers/ConfigurationController.h"

#include <utility>

// Owns the full request lifecycle: listens, dispatches work, and shuts down cleanly.
//
// Top-level orchestrator. Knows about every other layer, but no layer below it knows about Server.
class Server {
    public:
        Server();
        ~Server();

        /*
            Binds and listens on the given port (falling back to the PORT environment variable when no port is passed), then enters the accept loop.
            Prints a short startup banner.
        */
        void start(int port = 0);

        /*
            Signals the accept loop to exit and shuts down the ThreadPool gracefully, letting in-flight requests finish before returning..
        */
        void stop();

        const ServerConfig& getConfig() const;
        void updateConfig(const ServerConfig& newConfig);

    private:
        ServerConfig config;

        Socket listenSocket;
        ThreadPool pool;
        Router router;
        HealthController healthController;
        UserController userController;
        CorsMiddleware corsMiddleware;
        LoggerMiddleware loggerMiddleware;
        AuthMiddleware authMiddleware;
        RateLimiterMiddleware rateLimiterMiddleware;
        Logger logger;
        MetricsConnector metricsConnector;
        RequestHistory requestHistory;
        ServerEventPublisher events;
        ConfigurationController configurationController;

        bool running = false;
        
        /*
            The main loop: blocks on the listening Socket's accept(), wraps each new connection, and enqueues it on the ThreadPool rather than handling it inline — this is what makes the server concurrent.
        */
        void acceptConnections();

        /*
            Runs on a worker thread. Reads the raw request, parses it via HttpParser, runs it through the middleware chain, finds a matching Route via the Router, invokes its handler, and writes the resulting HttpResponse back through the Connection.
            Publishes RequestReceived/ResponseSent events along the way.
        */
        void handleConnection(Connection& conn);
}; 

#endif