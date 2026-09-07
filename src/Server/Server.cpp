# include "Server/Server.h"
#include <sstream>
#include <cstdlib>
#include <iostream>

void Server::start(int port) {
    if (port == 0) {
        const char* environmentPort = std::getenv("PORT");

        if (environmentPort != nullptr) {
            port = std::stoi(environmentPort);
        }
        else {
            port = 8080;
        }
    }

    listenSocket.bind(port); // Bind the socket
    listenSocket.listen(10); // Listening socket can queue up to roughly 10 pending connections while the server is busyy accepting/processing them.
    running = true;

    // Startup banner
    std::cout << "ForgeHTTP\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Server running on port " << port << '\n';
    std::cout << "Workers: 8\n";

    acceptConnections(); // Start accepting connections
}

void Server::stop() {
    running = false;
    // pool.shutdown() when implemented.
}

void Server::acceptConnections() {
    while (running) {
        Socket client = listenSocket.accept();

        // ThreadPool handling will be added later
    }
}

void Server::handleConnection(Connection& conn) {
}