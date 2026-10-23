# ForgeHTTP

**ForgeHTTP** is a lightweight HTTP/1.1 server and web application framework built from scratch in **C++20**.

The project focuses on building a practical, modular HTTP server while applying object-oriented design, networking, middleware architecture, routing, and modern C++ practices.

> **Goal:** Build a usable HTTP server in C++ that can serve real web applications while demonstrating the underlying engineering behind HTTP request handling and server architecture.

---

## Features

### HTTP Server

- HTTP/1.1 request handling with one request per TCP connection
- TCP socket-based networking
- RAII-based socket management
- Request parsing
- HTTP response generation
- JSON and plain-text responses
- HTTP status codes
- Request headers
- Query parameters
- Request bodies with validated `Content-Length`
- Request/header/body size limits and total read/write deadlines
- Correct response framing and explicit `400`, `404`, `405`, `408`, `413`, `429`, and `500` errors
- Graceful shutdown on `SIGINT` and `SIGTERM`
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

The server also uses an environment-configured bearer token, a bounded pending-work queue with overload responses, and SQLite-backed user persistence.

Middleware can either:

1. Modify or inspect the request/response and continue the chain.
2. Stop the request and return a response immediately.

For example, authentication middleware can reject an unauthenticated request with:

```text
401 Unauthorized
```

without allowing it to reach the router.
The server pipeline runs logging, optional CORS, rate limiting, optional authentication, and then routing. The auth middleware requires a configured Bearer token; it does not accept any arbitrary non-empty header.

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
   ├── Logger
   ├── CORS
   ├── Rate Limiter
   └── Authentication for protected /api/* routes
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
| CMake/CTest | Unit and real-socket integration tests |
| SQLite | Persistent demo API data |
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
- SQLite development headers/library (`libsqlite3-dev` on Ubuntu)
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
FORGEHTTP_API_TOKEN=local-demo-token PORT=8086 ./backend/build/forge
```

The server listens on port `8086` by default. Set `PORT` to use another port. Read-only dashboard endpoints (`GET /api/requests` and `GET /api/config`) are public. User operations and configuration changes require `FORGEHTTP_API_TOKEN`; if it is unset, protected API calls return `503 Service Unavailable` rather than accepting arbitrary credentials. User records are stored in `forgehttp.db` in the working directory by default. Set `FORGEHTTP_DB_PATH` to choose another SQLite file.

```bash
FORGEHTTP_API_TOKEN=local-demo-token \
FORGEHTTP_DB_PATH=./data/forgehttp.db \
PORT=8087 ./backend/build/forge
```

Run the backend unit and real-socket integration tests with:

```bash
./backend/build/forge_tests
./backend/build/forge_integration_tests
```

The integration suite starts a temporary server and exercises routes, error statuses, authentication, CRUD, persistence, framing limits, and concurrent clients.

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
Connection: close
Content-Length: 15

{"status":"ok"}
```

### Get Users

```bash
curl -H "Authorization: Bearer local-demo-token" \
  http://localhost:8086/api/users
```

### Create a User

```bash
curl -i -X POST http://localhost:8086/api/users \
  -H "Authorization: Bearer local-demo-token" \
  -H "Content-Type: application/json" \
  -d '{"name":"Motheo"}'
```

The response is `201 Created` with a generated ID and persisted user. Names must contain 1–100 bytes.

Individual users support `GET /api/users/{id}`, `PUT /api/users/{id}`, and `DELETE /api/users/{id}`:

```bash
curl -i -X PUT http://localhost:8086/api/users/1 \
  -H "Authorization: Bearer local-demo-token" \
  -H "Content-Type: application/json" \
  -d '{"name":"Updated name"}'

curl -i -X DELETE http://localhost:8086/api/users/1 \
  -H "Authorization: Bearer local-demo-token"
```

Invalid bodies return `400`, missing users return `404`, and successful deletes return `204 No Content`. The authorization header must be exactly `Bearer <FORGEHTTP_API_TOKEN>`. Use only a demo token in public screenshots and never commit real credentials.

### Metrics and request history

```bash
curl http://localhost:8086/metrics
curl -H "Authorization: Bearer local-demo-token" \
  http://localhost:8086/api/requests
```

Metrics include request count, active parsed requests, average response time, errors, and worker count. Request history is publicly readable and bounded by configuration; do not log sensitive information in URL paths.

### Calculator API

The standalone calculator in [`sites/`](./sites/) demonstrates a separate site using
ForgeHTTP as its backend. It sends arithmetic operations to a public, stateless API
endpoint:

```bash
curl -X POST http://localhost:8086/api/calculate \
  -H "Content-Type: application/json" \
  -d '{"left":24,"operator":"+","right":6}'
```

The JSON response contains the operands and result. Supported operators are `+`, `-`,
`*`, and `/`; invalid input and division by zero return `400`. It does not change user
or configuration data, though calculator calls are included in request metrics/history.
User operations and configuration changes remain protected.

`PUT /api/config` can update the rate limit, request-history capacity, CORS, authentication, and request logging at runtime. Port and worker-count changes are rejected with a restart-required message because they affect the listener and existing worker pool.

### Docker demo

From the repository root:

```bash
docker build -t forgehttp ./backend
docker run --rm -p 8086:8086 \
  -e FORGEHTTP_API_TOKEN=local-demo-token \
  -v forgehttp-data:/data \
  forgehttp
```

The image stores SQLite data under `/data`; the named volume keeps user records across container replacements.

### Dashboard

Start the React dashboard as described in [client/README.md](./client/README.md). Metrics, request history, and configuration are public; user operations and configuration changes require the admin bearer token entered through the sidebar sign-in modal. Dashboard history is retained by the browser without age-based trimming. The standalone calculator demo is in [`sites/`](./sites/README.md).

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

`ServerEventPublisher` publishes lifecycle and request events to `Logger` and `MetricsConnector`. The metrics snapshot is available at `/metrics`.

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

## Supported behavior and limitations

ForgeHTTP is a learning and portfolio project, not a production-hardened replacement for established servers. It intentionally implements a documented subset of HTTP:

- HTTP/1.1 origin-form requests with a `Host` header and `Content-Length` framing
- `GET`, `POST`, `PUT`, `DELETE`, `PATCH`, and `OPTIONS` method parsing
- One request per TCP connection; responses explicitly use `Connection: close`
- Request line limited to 8 KiB, headers to 32 KiB, and request body to 1 MiB
- Five-second total request-read and response-write deadlines
- No chunked request bodies, persistent keep-alive, TLS, virtual hosts, or static-file serving
- A bounded worker queue (128 waiting connections); overload receives `503`
- Shared demo bearer-token authentication, not user accounts or a production identity provider
- SQLite persistence intended for a small single-process demo, not multi-node scaling

The dashboard in `client/` connects to the hosted API and accepts its bearer token in the sidebar; the token stays in the current browser tab's session storage. The next protocol milestone is chunked transfer coding and keep-alive.

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

The backend includes bounded concurrent handling, request framing/size limits, routing, middleware, SQLite CRUD, lifecycle events, metrics, and unit/integration test targets. The React dashboard is configurable for a local demo. TLS, keep-alive, chunked request bodies, and static-file serving remain future milestones.

---

## Author

**Motheo Morena**

Software Developer

GitHub: [Motheo-365](https://github.com/Motheo-365)

---

## License

This project is currently intended primarily as a portfolio and learning project.