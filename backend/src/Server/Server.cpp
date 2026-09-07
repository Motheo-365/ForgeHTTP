#include "Server/Server.h"

#include "HTTP/HttpParser.h"
#include "HTTP/HttpResponse.h"
#include "HTTP/HttpRequest.h"
#include "Networking/Connection.h"

#include <cstdlib>
#include <iostream>
#include <memory>

Server::Server() : pool(8){
    router.get("/health", [this](const HttpRequest& req) {
        return healthController.getHealth(req);
    });

    router.get("/api/users", [this](const HttpRequest& req) {
        return userController.getUsers(req);
    });

    router.post("/api/users", [this](const HttpRequest& req) {
        return userController.createUser(req);
    });
}

void Server::start(int port) {
    if (port == 0) {
        const char* environmentPort = std::getenv("PORT");

        if (environmentPort != nullptr) {
            port = std::stoi(environmentPort);
        }
        else {
            port = 8085;
        }
    }

    listenSocket.bind(port);
    listenSocket.listen(10);
    running = true;

    std::cout << "\nServer running on port " << port << '\n';
    std::cout << "Workers: 8\n\n";

    acceptConnections();
}

void Server::stop() {
    running = false;
}

void Server::acceptConnections() {
    while (running) {
        Socket client = listenSocket.accept();

        auto connection = std::make_shared<Connection>(std::move(client));

        pool.enqueue(
            [this, connection]() {
                handleConnection(*connection);
            }
        );
    }
}

void Server::handleConnection(Connection& c) {
    try {
        std::string rawRequest = c.read();

        std::cout << "Received request:\n";
        std::cout << rawRequest << '\n';

        HttpRequest request = HttpParser::parse(rawRequest);

        std::cout << "Parsed path: "
                  << request.getPath()
                  << '\n';

        Route* route = router.match(request);

        if (route == nullptr) {
            HttpResponse response =
                HttpResponse::text("Route not found");

            response.setStatusCode(404);

            c.write(response.toString());
            c.close();
            return;
        }

        HttpResponse response =
            route->getHandler()(request);

        c.write(response.toString());
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