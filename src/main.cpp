#include "Networking/Socket.h"
#include "HTTP/HttpParser.h"
#include "HTTP/HttpResponse.h"

#include <iostream>
#include <exception>

void testServer();
void testHTTP();

int main() {
    std::cout << "ForgeHTTP starting...\n";

    testServer();
    testHTTP();

    return 0;
}

void testServer() {
    std::cout << "\n--- Socket Server Test ---\n";

    try {
        Socket server;
        std::cout << "Socket created successfully\n";
        server.bind(8080);
        std::cout << "Socket bound to port 8080\n";
        server.listen(10);
        std::cout << "Socket is listening\n";
    }

    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return;
    }

    std::cout << "Server socket going out of scope...\n";
}

void testHTTP() {
    try {
        std::string rawRequest =
            "GET /users?id=10 HTTP/1.1\r\n"
            "Host: localhost:8080\r\n"
            "Accept: application/json\r\n"
            "\r\n";

        HttpRequest request = HttpParser::parse(rawRequest);

        std::cout << "\n--- HTTP Request Test ---\n";

        std::cout << "Method: GET\n";
        std::cout << "Path: " << request.getPath() << '\n';
        std::cout << "Version: " << request.getVersion() << '\n';

        std::cout << "Host: " << request.getHeader("Host") << '\n';
        std::cout << "Accept: " << request.getHeader("accept") << '\n';
        std::cout << "ID: " << request.getQueryParam("id") << '\n';

        // -------------------------
        // HTTP Response Test
        // -------------------------

        std::cout << "\n--- HTTP Response Test ---\n";

        HttpResponse response = HttpResponse::json("{\"message\":\"Hello World\"}");

        response.setHeader("Server", "ForgeHTTP");
        std::cout << response.toString() << '\n';

        // -------------------------
        // Plain Text Response Test
        // -------------------------

        std::cout << "\n--- Text Response Test ---\n";

        HttpResponse textResponse = HttpResponse::text("Hello from ForgeHTTP");

        std::cout << textResponse.toString() << '\n';
    }

    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return;
    }
}