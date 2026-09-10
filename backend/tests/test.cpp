#include "Networking/Socket.h"

#include "HTTP/HttpParser.h"
#include "HTTP/HttpResponse.h"

#include "Middleware/Middleware.h"
#include "Middleware/LoggerMiddleware.h"
#include "Middleware/CorsMiddleware.h"
#include "Middleware/AuthMiddleware.h"
#include "Middleware/RateLimiterMiddleware.h"

#include <exception>
#include <iostream>
#include <string>

int testsPassed = 0;
int testsFailed = 0;

// --------------------------------------------------
// Test Helpers
// --------------------------------------------------

void pass(const std::string& testName) {
    std::cout << "[PASS] " << testName << '\n';
    ++testsPassed;
}

void fail(const std::string& testName, const std::string& reason = "") {
    std::cout << "[FAIL] " << testName;

    if (!reason.empty()) {
        std::cout << " - " << reason;
    }

    std::cout << '\n';
    ++testsFailed;
}

void section(const std::string& name) {
    std::cout << "\n--- " << name << " ---\n";
}

// --------------------------------------------------
// Test Declarations
// --------------------------------------------------

void testNetworking();
void testHTTPParser();
void testHTTPResponse();
void testMiddleware();

// --------------------------------------------------
// Main Test Runner
// --------------------------------------------------

int main() {
    std::cout
        << "=================== TESTING ForgeHTTP ====================\n";

    testNetworking();
    testHTTPParser();
    testHTTPResponse();
    testMiddleware();

    std::cout
        << "\n=================== TEST SUMMARY =========================\n";

    std::cout << "Tests passed: " << testsPassed << '\n';
    std::cout << "Tests failed: " << testsFailed << '\n';

    std::cout
        << "===========================================================\n";

    return testsFailed == 0 ? 0 : 1;
}

// --------------------------------------------------
// Networking Tests
// --------------------------------------------------

void testNetworking() {
    section("Networking / Socket");

    // Socket creation
    try {
        Socket server;

        pass("Socket can be created");

        // Bind
        try {
            server.bind(8085);

            pass("Socket can bind to port 8085");
        }
        catch (const std::exception& e) {
            fail(
                "Socket can bind to port 8085",
                e.what()
            );

            return;
        }

        // Listen
        try {
            server.listen(10);

            pass("Socket can listen");
        }
        catch (const std::exception& e) {
            fail(
                "Socket can listen",
                e.what()
            );

            return;
        }

        // RAII / destruction
        pass("Socket remains valid while in scope");
    }
    catch (const std::exception& e) {
        fail(
            "Socket can be created",
            e.what()
        );
    }

    // The Socket destructor is exercised when the object
    // above goes out of scope.
    pass("Socket can be destroyed through RAII");
}

// --------------------------------------------------
// HTTP Parser Tests
// --------------------------------------------------

void testHTTPParser() {
    section("HTTP Parser");

    try {
        std::string rawRequest =
            "GET /users?id=10&name=Motheo HTTP/1.1\r\n"
            "Host: localhost:8086\r\n"
            "Accept: application/json\r\n"
            "User-Agent: ForgeHTTP-Test\r\n"
            "\r\n";

        HttpRequest request = HttpParser::parse(rawRequest);

        // Path
        if (request.getPath() == "/users") {
            pass("Parser extracts request path");
        }
        else {
            fail(
                "Parser extracts request path",
                "Expected /users, got " + request.getPath()
            );
        }

        // HTTP version
        if (request.getVersion() == "HTTP/1.1") {
            pass("Parser extracts HTTP version");
        }
        else {
            fail(
                "Parser extracts HTTP version",
                "Expected HTTP/1.1, got " + request.getVersion()
            );
        }

        // Host header
        if (request.getHeader("Host") == "localhost:8086") {
            pass("Parser extracts Host header");
        }
        else {
            fail(
                "Parser extracts Host header",
                "Incorrect Host value"
            );
        }

        // Header lookup
        if (request.getHeader("accept") == "application/json") {
            pass("Header lookup is case-insensitive");
        }
        else {
            fail(
                "Header lookup is case-insensitive",
                "Accept header could not be retrieved"
            );
        }

        // Query parameter
        if (request.getQueryParam("id") == "10") {
            pass("Parser extracts query parameter");
        }
        else {
            fail(
                "Parser extracts query parameter",
                "Expected id=10"
            );
        }

        // Second query parameter
        if (request.getQueryParam("name") == "Motheo") {
            pass("Parser extracts multiple query parameters");
        }
        else {
            fail(
                "Parser extracts multiple query parameters",
                "Expected name=Motheo"
            );
        }

        // Missing query parameter
        if (request.getQueryParam("missing").empty()) {
            pass("Missing query parameter returns empty value");
        }
        else {
            fail(
                "Missing query parameter returns empty value"
            );
        }

        // Missing header
        if (request.getHeader("Missing-Header").empty()) {
            pass("Missing header returns empty value");
        }
        else {
            fail(
                "Missing header returns empty value"
            );
        }
    }
    catch (const std::exception& e) {
        fail(
            "HTTP request parsing",
            e.what()
        );
    }
}

// --------------------------------------------------
// HTTP Response Tests
// --------------------------------------------------

void testHTTPResponse() {
    section("HTTP Response");

    // JSON response
    try {
        HttpResponse response =
            HttpResponse::json("{\"message\":\"Hello World\"}");

        std::string output = response.toString();

        if (response.getStatusCode() == 200) {
            pass("JSON response defaults to HTTP 200");
        }
        else {
            fail(
                "JSON response defaults to HTTP 200"
            );
        }

        if (output.find("Content-Type: application/json") !=
            std::string::npos) {
            pass("JSON response sets application/json content type");
        }
        else {
            fail(
                "JSON response sets application/json content type"
            );
        }

        if (output.find("Content-Length: 25") !=
            std::string::npos) {
            pass("JSON response calculates Content-Length");
        }
        else {
            fail(
                "JSON response calculates Content-Length"
            );
        }

        if (output.find("{\"message\":\"Hello World\"}") !=
            std::string::npos) {
            pass("JSON response contains response body");
        }
        else {
            fail(
                "JSON response contains response body"
            );
        }
    }
    catch (const std::exception& e) {
        fail(
            "JSON response creation",
            e.what()
        );
    }

    // Plain text response
    try {
        HttpResponse response =
            HttpResponse::text("Hello from ForgeHTTP");

        std::string output = response.toString();

        if (response.getStatusCode() == 200) {
            pass("Text response defaults to HTTP 200");
        }
        else {
            fail(
                "Text response defaults to HTTP 200"
            );
        }

        if (output.find("Content-Type: text/plain") !=
            std::string::npos) {
            pass("Text response sets text/plain content type");
        }
        else {
            fail(
                "Text response sets text/plain content type"
            );
        }

        if (output.find("Hello from ForgeHTTP") !=
            std::string::npos) {
            pass("Text response contains response body");
        }
        else {
            fail(
                "Text response contains response body"
            );
        }
    }
    catch (const std::exception& e) {
        fail(
            "Text response creation",
            e.what()
        );
    }

    // Custom header
    try {
        HttpResponse response =
            HttpResponse::text("Hello");

        response.setHeader("Server", "ForgeHTTP");

        std::string output = response.toString();

        if (output.find("Server: ForgeHTTP") !=
            std::string::npos) {
            pass("Response accepts custom headers");
        }
        else {
            fail(
                "Response accepts custom headers"
            );
        }
    }
    catch (const std::exception& e) {
        fail(
            "Response accepts custom headers",
            e.what()
        );
    }

    // 404
    try {
        HttpResponse response =
            HttpResponse::text("Page not found");

        response.setStatusCode(404);

        std::string output = response.toString();

        if (response.getStatusCode() == 404) {
            pass("Response status can be changed to 404");
        }
        else {
            fail(
                "Response status can be changed to 404"
            );
        }

        if (output.find("HTTP/1.1 404 Not Found") !=
            std::string::npos) {
            pass("404 response contains correct status line");
        }
        else {
            fail(
                "404 response contains correct status line"
            );
        }
    }
    catch (const std::exception& e) {
        fail(
            "404 response",
            e.what()
        );
    }

    // 401
    try {
        HttpResponse response =
            HttpResponse::text("Unauthorized");

        response.setStatusCode(401);

        std::string output = response.toString();

        if (output.find("HTTP/1.1 401 Unauthorized") !=
            std::string::npos) {
            pass("401 response contains correct status line");
        }
        else {
            fail(
                "401 response contains correct status line"
            );
        }
    }
    catch (const std::exception& e) {
        fail(
            "401 response",
            e.what()
        );
    }

    // 429
    try {
        HttpResponse response =
            HttpResponse::text("Too many requests");

        response.setStatusCode(429);

        std::string output = response.toString();

        if (output.find("HTTP/1.1 429 Too Many Requests") !=
            std::string::npos) {
            pass("429 response contains correct status line");
        }
        else {
            fail(
                "429 response contains correct status line"
            );
        }
    }
    catch (const std::exception& e) {
        fail(
            "429 response",
            e.what()
        );
    }
}

// --------------------------------------------------
// Middleware Tests
// --------------------------------------------------

void testMiddleware() {
    section("Middleware");

    // ----------------------------------------------
    // Auth Middleware - Missing Authorization
    // ----------------------------------------------

    try {
        std::string rawRequest =
            "GET /api/users HTTP/1.1\r\n"
            "Host: localhost:8086\r\n"
            "\r\n";

        HttpRequest request =
            HttpParser::parse(rawRequest);

        HttpResponse response;

        AuthMiddleware auth;

        bool nextCalled = false;

        auth.handle(
            request,
            response,
            [&nextCalled]() {
                nextCalled = true;
            }
        );

        if (response.getStatusCode() == 401) {
            pass("Auth middleware rejects missing credentials");
        }
        else {
            fail(
                "Auth middleware rejects missing credentials",
                "Expected status 401"
            );
        }

        if (!nextCalled) {
            pass("Auth middleware stops unauthorized request");
        }
        else {
            fail(
                "Auth middleware stops unauthorized request"
            );
        }
    }
    catch (const std::exception& e) {
        fail(
            "Auth middleware missing credentials",
            e.what()
        );
    }

    // ----------------------------------------------
    // Auth Middleware - Authorization Present
    // ----------------------------------------------

    try {
        std::string rawRequest =
            "GET /api/users HTTP/1.1\r\n"
            "Host: localhost:8086\r\n"
            "Authorization: Bearer test-token\r\n"
            "\r\n";

        HttpRequest request =
            HttpParser::parse(rawRequest);

        HttpResponse response;

        AuthMiddleware auth;

        bool nextCalled = false;

        auth.handle(
            request,
            response,
            [&nextCalled]() {
                nextCalled = true;
            }
        );

        if (nextCalled) {
            pass("Auth middleware allows request with credentials");
        }
        else {
            fail(
                "Auth middleware allows request with credentials"
            );
        }
    }
    catch (const std::exception& e) {
        fail(
            "Auth middleware with credentials",
            e.what()
        );
    }

    // ----------------------------------------------
    // Middleware object creation
    // ----------------------------------------------

    try {
        LoggerMiddleware logger;
        CorsMiddleware cors;
        RateLimiterMiddleware rateLimiter(100);

        (void)logger;
        (void)cors;
        (void)rateLimiter;

        pass("Logger middleware can be created");
        pass("CORS middleware can be created");
        pass("Rate limiter middleware can be created");
    }
    catch (const std::exception& e) {
        fail(
            "Middleware objects can be created",
            e.what()
        );
    }
}