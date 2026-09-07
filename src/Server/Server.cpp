#include "Server/Server.h"

#include "HTTP/HttpParser.h"
#include "HTTP/HttpResponse.h"
#include "HTTP/HttpRequest.h"
#include "Networking/Connection.h"

#include <cstdlib>
#include <iostream>
#include <memory>

Server::Server() : pool(8) {}

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

        std::cout << "Received request:\n";
        std::cout << rawRequest << '\n';

        std::cout << "Parsed path: " << request.getPath() << '\n';

        HttpResponse response = HttpResponse::text("Hello from ForgeHTTP!\n");

        c.write(response.toString());
    }

    catch (const std::exception& e) {
        std::cerr << "Request error: " << e.what() << '\n';
        HttpResponse response = HttpResponse::text("Bad Request");
        response.setSTatusCode(400);
        c.write(response.toString());
    }

    c.close();
}