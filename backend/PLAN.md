# ForgeHTTP — Project Plan

A lightweight HTTP/1.1 web server and application framework built from scratch in modern C++, with a React admin dashboard as the frontend layer.

**Goal:** Not to compete with Nginx — to demonstrate real command of networking, C++, OOP, concurrency, and software architecture, in a project that's finishable, deployable, and defensible in an interview.

The repository's source headers and tests are the current implementation contract. This plan records the architecture, completed backend work, and the next backend milestones.

---

## Table of Contents

1. [Vision & Success Criteria](#1-vision--success-criteria)
2. [Tech Stack](#2-tech-stack)
3. [Architecture](#3-architecture)
4. [Repository Structure](#4-repository-structure)
5. [Core Classes](#5-core-classes)
6. [Design Patterns](#6-design-patterns)
7. [Observability](#7-observability)
8. [Frontend: Admin Dashboard](#8-frontend-admin-dashboard)
9. [Deployment](#9-deployment)
10. [Milestones / Roadmap](#10-milestones--roadmap)
11. [Immediate Next Steps](#11-immediate-next-steps)

---

## 1. Vision & Success Criteria

The current backend can be started with:

```bash
cmake -S backend -B backend/build
cmake --build backend/build
FORGEHTTP_API_TOKEN=local-demo-token PORT=8086 ./backend/build/forge
```

The default port is `8086`. Read-only dashboard endpoints (`GET /api/requests` and
`GET /api/config`) are public. User operations and configuration changes require
`FORGEHTTP_API_TOKEN`.

This starts a concurrent HTTP server that:

- Parses raw HTTP/1.1 requests over POSIX sockets (no Boost.Beast, no framework doing the real work)
- Routes requests — including path parameters like `/users/:id` — through a middleware chain
- Serves a small JSON API
- Exposes a `/metrics` endpoint
- Can be consumed by the React dashboard in `client/`
- Limits request/header sizes and socket deadlines, and closes after one request per connection
- Stores user records in SQLite and supports authenticated CRUD
- Bounds the work queue and returns `503` when overloaded

### Definition of done

The project is successful if I can explain, unaided, in an interview:

- How `accept()` and the socket lifecycle work
- Why the thread pool needs a condition variable, and how the task queue avoids corruption
- How the HTTP parser turns raw bytes into a request object
- Why middleware is Chain of Responsibility, and why the socket wrapper uses RAII

---

## 2. Tech Stack

| Concern | Choice | Why |
|---|---|---|
| Language | C++20 | Modern features without hiding fundamentals |
| Build system | CMake | Industry standard, plays well with Docker |
| Testing | CMake/CTest | Unit tests plus real-socket server integration tests |
| Networking | POSIX sockets (`socket`, `bind`, `listen`, `accept`, `recv`, `send`) | Forces understanding of what HTTP sits on top of |
| JSON | nlohmann/json | Request validation and response serialization |
| Frontend | React (+ a chart lib, e.g. Recharts) | Reuses existing React experience; talks to the C++ server's own API |
| Deployment | Docker | Reproducible Linux build; Docker Compose remains a follow-up |

Dependencies are kept deliberately minimal. The interesting parts — sockets, HTTP parsing, concurrency, routing — are built by hand, not imported.

---

## 3. Architecture

```
CLIENT
  │
  ▼
TCP Socket → HTTP Parser → Thread Pool → Middleware → Router → Controller → Response
  │                                                                             │
  └─────────────────────────────────────────────────────────────────────────────┘
                                    back to CLIENT
```

Each layer only knows about the layer directly below it:

- **Server** coordinates lifecycle, request dispatch, middleware, routing, and observability.
- **Socket** is a thin RAII wrapper around a file descriptor.
- **HttpParser** turns raw bytes into an `HttpRequest`; it knows nothing about routing.
- **Router** maps method + path to a handler; it knows nothing about sockets.
- **Middleware** runs before routing: CORS, logger, rate limiter, and auth for protected `/api/*` routes. Read-only dashboard history/config GET routes are public.
- **Observability** is cross-cutting, not part of the request pipeline: `Server` owns a `ServerEventPublisher` and publishes lifecycle/request events to it (connection accepted, response sent, server started/stopped); `Logger` and `MetricsConnector` subscribe independently and react without `Server` knowing who's listening.

---

## 4. Repository Structure

```
ForgeHTTP/
├── README.md
├── backend/
│   ├── CMakeLists.txt
│   ├── Dockerfile
│   ├── PLAN.md
│   ├── include/           Public C++ headers by layer
│   ├── src/               C++ implementations and main.cpp
│   ├── tests/test.cpp     Backend regression tests
│   └── build/             Generated CMake output
└── client/                React dashboard
```

---

## 5. Core Classes

The source headers and tests define the detailed method contracts. This section is the short version.

### `Socket` — RAII wrapper

```cpp
class Socket {
public:
    Socket();
    ~Socket();               // closes the fd automatically
    void bind(int port);
    void listen(int backlog);
    Socket accept();
    void close();
    std::string receive();
    void send(const std::string& data);
private:
    int fileDescriptor;
};
```

The destructor closing the fd is the concrete demonstration of RAII — no manual `close()` calls scattered through the codebase.

### `HttpRequest`

Holds: `method`, `path`, `version`, `headers`, `body`, `query parameters`.
Example: `GET /users?id=10 HTTP/1.1` → `method=GET`, `path=/users`, `query["id"]="10"`.

### `HttpResponse`

Holds: `status code`, `headers`, `body`. Provides helpers like `HttpResponse::json(...)` that generate a full HTTP response with correct `Content-Type` and `Content-Length`.

### `Router`

```cpp
router.get("/hello", helloHandler);
router.get("/users", getUsers);
router.post("/users", createUser);
```

Supports path parameters (`/users/:id` → `params["id"] = "42"`).

### `ThreadPool`

```cpp
ThreadPool pool(8);
pool.enqueue([] { handleRequest(); });
```

Built with `std::thread`, `std::mutex`, `std::condition_variable`, and `std::atomic`. Replaces a naive "spawn a thread per connection" model with a fixed pool pulling work off a shared task queue.

### `Middleware`

Chain of Responsibility: each middleware receives the request, does its work, and decides whether to call the next link.

```
Request → CORS → Logger → Rate Limiter → Auth for protected /api/* → Router
```

---

## 6. Design Patterns

Used deliberately, not for a checklist:

| Pattern | Where | Why it's genuinely needed |
|---|---|---|
| RAII | `Socket`, locks, connections | C++-idiomatic resource safety |
| Factory | Building responses/handlers | Decouples construction from use |
| Strategy | Routing strategies (exact / prefix / parameter match) | Different route-matching algorithms, swappable |
| Chain of Responsibility | Middleware pipeline | Each middleware independently decides to continue or short-circuit |
| Observer | Server events (`RequestReceived`, `ResponseSent`, `ServerStarted`/`Stopped`) | Logger, metrics, and monitoring subscribe independently |

Aiming for 4–5 patterns that solve a real problem, not ten patterns for the sake of a portfolio bullet point.

---

## 7. Observability

Structured request logging:

```
[18:42:10] GET /api/users       200  12ms
[18:42:11] GET /api/users/42    200   4ms
[18:42:12] POST /api/users      201  17ms
[18:42:15] GET /does-not-exist  404   2ms
```

A `/metrics` endpoint exposing:

```json
{
  "requests": 12452,
  "active_connections": 17,
  "average_response_time_ms": 8.4,
  "errors": 31,
  "workers": 8
}
```

This is what elevates the pitch from "I made a server" to "I built a concurrent HTTP server with observability" — and it's what the dashboard will visualize.

---

## 8. Frontend: Admin Dashboard

The React dashboard in `client/` consumes the C++ server's health, metrics, request-history, user, and configuration APIs; it does not need a separate backend.

The dashboard includes:

- Header: server name/status
- Stat cards: total requests, active connections, average response time
- Live metrics charts, refreshed by polling
- Recent request history (method, path, status, latency)
- API/configuration pages for exercising the server

The dashboard connects to the hosted API and accepts a bearer token in its sidebar for protected endpoints. For local development against a different server, update the API URL in `client/src/services/api.ts`; enter the accepted bearer token in the dashboard sidebar. The token is stored only in the current browser tab's session storage.

---

## 9. Deployment

Designed in from day one, not retrofitted:

- `Dockerfile` — builds via CMake inside Ubuntu, runs CTest, and produces the `forge` binary
- Docker Compose orchestration — planned, not currently included
- Port is configurable (default `8086`):

```cpp
const char* port = std::getenv("PORT");
// fall back to 8086 if unset
```

---

## 10. Milestones / Roadmap

| # | Milestone | Outcome |
|---|---|---|
| 1 | TCP server | Implemented: RAII sockets, deadlines, bounded queue, overload response, and graceful shutdown |
| 2 | HTTP parsing | Implemented subset: HTTP/1.1 headers, decoded query strings, strict `Content-Length`, and limits; no chunked bodies |
| 3 | Router | Implemented: method matching, path parameters, 404, and 405/Allow responses |
| 4 | Thread pool | Implemented: fixed workers, bounded thread-safe queue, drain-and-join shutdown |
| 5 | Middleware | Implemented: configurable CORS/logging/auth and thread-safe per-client rate limiting |
| 6 | Static files | Planned: serve HTML/CSS/JS from a configurable directory |
| 7 | JSON API | Implemented: health endpoint and authenticated SQLite-backed user CRUD |
| 8 | Metrics | Implemented: `/metrics`, lifecycle/request events, request history, timings, errors, and workers |
| 9 | React dashboard | Implemented: `client/` connects to live health, metrics, request history, and configuration APIs |
| 10 | Docker + deployment | Implemented: SQLite-aware Docker image and GitHub Actions CI added; CMake build and CTest pass in Ubuntu WSL; Docker/hosted CI execution remains |

**Rule for the whole project:** build bottom-up. Don't skip ahead to framework niceties before the TCP/HTTP fundamentals are solid — the fundamentals are the actual point of the project.

---

## 11. Immediate Next Steps

1. Run the Docker build and GitHub Actions workflow; the backend CMake build and CTest suites already pass in Ubuntu WSL.
2. Expand HTTP interoperability with keep-alive and chunked transfer coding, preserving strict parser tests.
3. Add keep-alive and chunked transfer coding with integration coverage.
4. Consider static-file serving only after HTTP behavior is well covered by integration tests.
