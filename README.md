# ForgeHTTP

**ForgeHTTP** is a lightweight HTTP/1.1 server and web application framework built from scratch in **C++20**.

The project focuses on building a practical, modular HTTP server while applying object-oriented design, networking, middleware architecture, routing, and modern C++ practices.

> **Goal:** Build a usable HTTP server in C++ that can serve real web applications while demonstrating the underlying engineering behind HTTP request handling and server architecture.

---

## Features

### HTTP Server

- HTTP/1.1 request handling
- TCP socket-based networking
- RAII-based socket management
- Request parsing
- HTTP response generation
- JSON and plain-text responses
- HTTP status codes
- Request headers
- Query parameters
- Request bodies with `Content-Length`
- `OPTIONS` and CORS preflight handling

### Routing

ForgeHTTP provides a simple routing system for registering HTTP handlers:

- `GET`, `POST`, `PUT`, `DELETE`, `PATCH`, and `OPTIONS`

Example:

```cpp
router.get("/health", [](const HttpRequest& req) {
   return HttpResponse::json("{\"status\":\"ok\"}");
});
```

### Middleware

Requests pass through the server's middleware pipeline before reaching the router.

Current middleware includes:

- Logger Middleware
- CORS Middleware
- Authentication Middleware
- Rate Limiter Middleware

Middleware can either:

1. Modify or inspect the request/response and continue the chain.
2. Stop the request and return a response immediately.

For example, authentication middleware can reject an unauthenticated request with:

```text
401 Unauthorized
```

without allowing it to reach the router.

### Server Architecture

ForgeHTTP separates responsibilities into distinct components:

```text
Client
   │
   ▼
TCP Socket
   │
   ▼
HTTP Parser
   │
   ▼
Middleware Chain
   │
   ├── CORS
   ├── Logger
   ├── Rate Limiter
   └── Authentication for /api/*
   │
   ▼
Router
   │
   ▼
Route Handler
   │
   ▼
HTTP Response
   │
   ▼
Client
```

This separation allows individual components to be developed and tested independently.

---

## Tech Stack

| Technology | Purpose |
|---|---|
| C++20 | Core language |
| CMake | Build system |
| POSIX Sockets | TCP networking |
| HTTP/1.1 | Application protocol |
| nlohmann/json | JSON parsing and serialization |
| CMake test executable | Backend regression tests |
| Git/GitHub | Version control |
| React | Web dashboard |

---

## Project Structure

```text
ForgeHTTP/
├── README.md
├── backend/
│   ├── CMakeLists.txt
│   ├── Dockerfile
│   ├── PLAN.md
│   ├── include/
│   │   ├── HTTP/
│   │   ├── Middleware/
│   │   ├── Networking/
│   │   ├── Observability/
│   │   ├── Routing/
│   │   └── Server/
│   ├── src/
│   │   ├── HTTP/
│   │   ├── Middleware/
│   │   ├── Networking/
│   │   ├── Observability/
│   │   ├── Routing/
│   │   ├── Server/
│   │   └── main.cpp
│   └── tests/test.cpp
└── client/
   └── React dashboard
```

---

## Building

### Requirements

- Linux / WSL
- C++20-compatible compiler
- CMake 3.20+
- Git

### Clone the repository

```bash
git clone https://github.com/Motheo-365/ForgeHTTP.git
cd ForgeHTTP
```

### Configure the project

```bash
cmake -S backend -B backend/build
```

### Build

```bash
cmake --build backend/build
```

### Run

```bash
./backend/build/forge
```

The server listens on port `8085` by default. Set `PORT` to use another port:

```bash
PORT=8086 ./backend/build/forge
```

Run the backend tests with:

```bash
./backend/build/forge_tests
```

---

## Example API

### Health Check

```http
GET /health HTTP/1.1
Host: localhost:8086
```

Response:

```http
HTTP/1.1 200 OK
Content-Type: application/json

{"status":"ok"}
```

### Get Users

```http
GET /api/users HTTP/1.1
Host: localhost:8086
Authorization: Bearer test-token
```

Example response:

```json
[
    {
        "id": 1,
        "name": "Alice"
    },
    {
        "id": 2,
        "name": "Bob"
    }
]
```

`/api/users` requires a non-empty `Authorization` header. Requests without one receive `401 Unauthorized`.

### Create User

```http
POST /api/users HTTP/1.1
Host: localhost:8086
Content-Type: application/json
Authorization: Bearer test-token
```

Example request body:

```json
{
    "name": "Motheo"
}
```

Response:

```http
HTTP/1.1 201 Created
```

### Metrics

```bash
curl http://localhost:8086/metrics
```

The response includes request count, active connections, average response time, error count, and worker count.

---

## Middleware

ForgeHTTP uses the **Chain of Responsibility** pattern for request middleware.

Each middleware receives:

```cpp
void handle(
    HttpRequest& req,
    HttpResponse& res,
    std::function<void()> next
);
```

A middleware can continue processing by calling:

```cpp
next();
```

or terminate the request by returning without calling `next()`.

For example:

```text
Request
   │
   ▼
CORS
   │
   ▼
Logger
   │
   ▼
Authentication
   │
   ├── Invalid → 401
   │
   └── Valid
        │
        ▼
   Rate Limiter
        │
        ├── Limit exceeded → 429
        │
        └── Allowed
              │
              ▼
            Router
```

This provides a clean way to add cross-cutting functionality without coupling it directly to route handlers.

---

## Design & Architecture

ForgeHTTP is designed around separation of responsibilities.

### Socket Layer

Responsible for low-level TCP networking:

- Creating sockets
- Binding
- Listening
- Accepting connections
- Receiving data
- Sending data
- Resource cleanup

### HTTP Layer

Responsible for understanding HTTP:

- HTTP methods
- Request parsing
- Headers
- Query parameters
- Request bodies
- Response construction
- Status codes

### Middleware Layer

Responsible for cross-cutting request processing:

- Logging
- CORS
- Authentication
- Rate limiting

### Routing Layer

Responsible for mapping an HTTP request to an application handler.

### Server Layer

Coordinates the networking, HTTP, middleware, and routing components.

### Observability Layer

`ServerEventPublisher` publishes lifecycle and request events to `Logger` and `MetricsCollector`. The metrics snapshot is available at `/metrics`.

---

## Design Patterns

ForgeHTTP intentionally applies established software design principles and patterns where they provide practical value.

### RAII

Sockets use C++ RAII to ensure system resources are released automatically when the socket object goes out of scope.

### Chain of Responsibility

Middleware is implemented as a chain where each middleware decides whether to continue processing the request.

### Strategy

The architecture is designed to allow request-handling behaviour to be composed without tightly coupling the server to individual implementations.

### Factory

Factories can be used where object creation becomes sufficiently complex to justify separating construction from usage.

The project aims to use design patterns where they solve an actual architectural problem rather than adding abstraction for its own sake.

---

## Development Goals

ForgeHTTP is being developed incrementally, with an emphasis on understanding and implementing each layer rather than relying on an existing web-server framework.

Current development areas include:

- HTTP request/response handling
- Middleware pipeline
- Routing
- Server architecture
- Concurrent request handling
- React-based administration dashboard
- Static file serving
- Stronger authentication and configurable rate-limit windows
- More complete HTTP/1.1 behavior, including keep-alive and chunked bodies

---

## Why ForgeHTTP?

ForgeHTTP was created to apply C++ to a project that can actually be used on the web.

Rather than building another isolated C++ exercise, the project explores how a real web server can be structured from the networking layer upward.

The project combines:

- Systems programming
- Network programming
- HTTP
- Object-oriented design
- Software architecture
- Concurrency
- Web development

The result is a project that demonstrates both **low-level C++ development** and **practical web application engineering**.

---

## Status

🚧 **Active Development**

The backend builds and runs with concurrent request handling, routing, middleware, JSON API endpoints, lifecycle events, and metrics. The React dashboard and broader HTTP functionality are still under development.

---

## Author

**Motheo Morena**

Aspiring Software Developer

GitHub: [Motheo-365](https://github.com/Motheo-365)

---

## License

This project is currently intended primarily as a portfolio and learning project.