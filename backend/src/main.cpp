#include "Server/Server.h"

#include <exception>
#include <iostream>

int main() {
    std::cout << "ForgeHTTP starting...\n";

    try {
        Server server;
        server.start();
    }

    catch (const std::exception& e) {
        std::cerr << "ForgeHTTP error: "
                  << e.what()
                  << '\n';

        return 1;
    }

    return 0;
}