# ForgeHTTP Framework Expansion Plan

## 1. Vision

Expand ForgeHTTP from a lightweight HTTP server into a small C++ web framework that allows developers to build simple websites and web applications using C++ for the backend and standard web technologies for the frontend.

Forge should provide the infrastructure needed to:

- Start an HTTP server
- Define routes
- Handle HTTP requests and responses
- Serve HTML, CSS, JavaScript, images, and other static files
- Create reusable application handlers
- Use middleware
- Read URL parameters and query parameters
- Return JSON responses
- Render simple HTML pages
- Connect browser-based frontends to C++ backend logic
- Organize applications into a clear project structure

The framework should remain lightweight and understandable.

The goal is **not** to compete with large frameworks such as Django, Laravel, Express, or ASP.NET.

The goal is to demonstrate how a web framework can be designed and implemented in C++ while still being useful for building small real-world websites and APIs.

---

# 2. Core Development Model

Forge applications should use **standard web technologies for the frontend** and **C++ for the backend**.

Developers should NOT have to write HTML tags directly inside C++ for normal website development.

Instead:

```text
Frontend
├── HTML
├── CSS
└── JavaScript

Backend
└── C++ / Forge
```

Forge connects the two sides.

Example:

```text
Browser
   │
   │ GET /
   ▼
Forge
   │
   ▼
public/index.html
   │
   ▼
Browser renders website
```

For dynamic functionality:

```text
Browser
   │
   │ GET /api/users
   ▼
Forge
   │
   ▼
C++ route handler
   │
   ▼
JSON response
   │
   ▼
JavaScript updates webpage
```

This means a developer can use familiar web development techniques while using C++ as the server-side language.

---

# 3. Target Developer Experience

A developer should eventually be able to create a Forge application with a structure such as:

```text
my-website/
│
├── CMakeLists.txt
│
├── src/
│   └── main.cpp
│
├── public/
│   ├── index.html
│   ├── about.html
│   ├── style.css
│   ├── script.js
│   └── images/
│
└── templates/
```

The developer writes their website normally.

Example `index.html`:

```html
<!DOCTYPE html>
<html>
<head>
    <title>My Forge Website</title>
    <link rel="stylesheet" href="/style.css">
</head>

<body>
    <nav>
        <a href="/">Home</a>
        <a href="/about">About</a>
    </nav>

    <main>
        <h1>Hello from Forge!</h1>
        <p>This website is powered by a C++ backend.</p>
    </main>
</body>
</html>
```

C++ defines the server-side behaviour:

```cpp
Forge::App app;

app.get("/", [](const Forge::Request& req,
                Forge::Response& res)
{
    res.file("public/index.html");
});

app.listen(8080);
```

The exact API can change during development. The important principle is that **HTML remains HTML and C++ remains backend/application code**.

---

# 4. Architecture

Forge should eventually be divided into two conceptual layers.

```text
ForgeHTTP
│
├── HTTP Server
│   ├── Socket
│   ├── Connection
│   ├── HTTP Parser
│   └── HTTP Response
│
└── Framework
    ├── Application
    ├── Router
    ├── Request
    ├── Response
    ├── Middleware
    ├── Static Files
    ├── Templates
    └── Utilities
```

The lower layer handles networking and HTTP.

The upper layer provides the developer-facing framework API.

---

# 5. Phase 1 — Stabilise the HTTP Server

Before expanding into a framework, make sure the existing HTTP server is reliable.

### Tasks

- [ ] Finalise `HttpRequest`
- [ ] Finalise `HttpResponse`
- [ ] Finalise HTTP status handling
- [ ] Finalise HTTP headers
- [ ] Improve HTTP parsing
- [ ] Handle malformed requests
- [ ] Handle unsupported HTTP methods
- [ ] Handle unknown routes
- [ ] Handle request bodies
- [ ] Handle `Content-Length`
- [ ] Test multiple requests
- [ ] Test concurrent connections
- [ ] Clean up socket ownership
- [ ] Document the networking layer

### Expected result

ForgeHTTP should reliably perform:

```text
Client
  ↓
Socket
  ↓
HTTP Parser
  ↓
Request
  ↓
Router
  ↓
Handler
  ↓
Response
  ↓
Socket
```

---

# 6. Phase 2 — Create the Application API

Introduce a high-level application class.

Possible structure:

```text
include/
└── Forge/
    ├── App.h
    ├── Request.h
    ├── Response.h
    └── Router.h
```

The `App` class becomes the main entry point for developers.

Conceptually:

```cpp
Forge::App app;
```

The application should manage:

- Routes
- Middleware
- Server configuration
- Request dispatching
- Static files
- Application lifecycle

### Tasks

- [ ] Create `App`
- [ ] Allow developers to configure the server
- [ ] Allow developers to register routes
- [ ] Allow developers to register middleware
- [ ] Add `listen(port)`
- [ ] Connect `App` to the existing server implementation
- [ ] Hide low-level socket functionality from application developers

---

# 7. Phase 3 — Improve the Router

The router is one of the most important parts of the framework.

Support:

```text
GET
POST
PUT
PATCH
DELETE
```

Example:

```text
GET  /
GET  /about
GET  /users
POST /users
```

### Route Parameters

Eventually support:

```text
/users/:id
```

so:

```text
/users/42
```

produces:

```text
id = 42
```

### Query Parameters

Support:

```text
/search?q=forge&page=2
```

and expose:

```text
q    → forge
page → 2
```

### Data structure

Initially use a simple hash-based route structure.

Possible future improvement:

```text
Router
   ↓
Route Tree / Trie
```

Do not implement a complex routing tree until the basic router works.

### Tasks

- [ ] Route registration
- [ ] HTTP method matching
- [ ] Path matching
- [ ] Route parameters
- [ ] Query parameters
- [ ] 404 handling
- [ ] 405 Method Not Allowed handling
- [ ] Investigate trie/radix-tree routing later

---

# 8. Phase 4 — Request and Response API

Make requests and responses convenient for framework developers.

## Request

A request should expose:

```text
Method
Path
Headers
Query Parameters
Route Parameters
Body
```

Conceptually:

```text
request.method()
request.path()
request.header("Content-Type")
request.query("page")
request.param("id")
request.body()
```

## Response

The response should provide:

```text
response.status(200)
response.header("Content-Type", "text/html")
response.send("Hello")
```

Higher-level helpers:

```text
response.html(...)
response.json(...)
response.file(...)
response.redirect(...)
```

### Tasks

- [ ] Clean request API
- [ ] Clean response API
- [ ] Header access
- [ ] Query parameter access
- [ ] Route parameter access
- [ ] Response status helper
- [ ] Response header helper
- [ ] HTML response helper
- [ ] JSON response helper
- [ ] Static file response helper
- [ ] Redirect helper

---

# 9. Phase 5 — Static Website Support

This is a core feature of Forge.

Forge should allow developers to create websites using normal:

- HTML
- CSS
- JavaScript
- Images
- Fonts
- Other static assets

Example:

```text
my-site/
│
├── src/
│   └── main.cpp
│
└── public/
    ├── index.html
    ├── about.html
    ├── style.css
    ├── script.js
    └── images/
        └── logo.png
```

Forge should be able to expose the `public` directory.

For example:

```text
/
      → public/index.html

/about.html
      → public/about.html

/style.css
      → public/style.css

/script.js
      → public/script.js

/images/logo.png
      → public/images/logo.png
```

### Tasks

- [ ] Create static file middleware
- [ ] Map URL paths to filesystem paths
- [ ] Determine MIME types
- [ ] Handle missing files
- [ ] Prevent path traversal
- [ ] Support common file types
- [ ] Add configurable public directory
- [ ] Add automatic `index.html` handling

### Goal

A developer should be able to build a complete small static website without writing HTML inside C++.

---

# 10. Phase 6 — Frontend + C++ API Integration

Allow normal frontend JavaScript to communicate with C++ routes.

Example frontend:

```text
JavaScript
   │
   │ fetch("/api/users")
   ▼
Forge Router
   │
   ▼
C++ Handler
   │
   ▼
JSON
   │
   ▼
JavaScript
```

This allows developers to build more interactive websites.

For example:

```text
public/
├── index.html
├── style.css
└── app.js
```

`app.js` can call:

```text
GET /api/users
POST /api/users
DELETE /api/users/:id
```

while Forge handles the server-side logic.

### Tasks

- [ ] JSON response support
- [ ] JSON request body support
- [ ] CORS middleware
- [ ] API route organisation
- [ ] Example `fetch()` integration
- [ ] Document frontend/backend communication

---

# 11. Phase 7 — Middleware System

Build on the middleware system already present in ForgeHTTP.

Middleware should be able to run before and/or after request handling.

Example:

```text
Request
   ↓
Logger
   ↓
CORS
   ↓
Authentication
   ↓
Rate Limiter
   ↓
Router
   ↓
Handler
   ↓
Response
```

Potential framework API:

```text
app.use(LoggerMiddleware());
app.use(CorsMiddleware());
app.use(RateLimiterMiddleware());
```

### Tasks

- [ ] Define middleware interface
- [ ] Middleware chaining
- [ ] Request continuation
- [ ] Middleware short-circuiting
- [ ] Logger middleware
- [ ] CORS middleware
- [ ] Authentication middleware
- [ ] Rate limiter middleware

---

# 12. Phase 8 — JSON API Support

Forge should support small REST APIs as well as websites.

Example:

```text
GET    /api/users
GET    /api/users/:id
POST   /api/users
DELETE /api/users/:id
```

Response:

```json
{
    "id": 1,
    "name": "Alice"
}
```

### Tasks

- [ ] JSON response support
- [ ] JSON request body support
- [ ] JSON content-type handling
- [ ] Basic serialization/deserialization
- [ ] Error responses
- [ ] API route grouping

Keep JSON support optional and lightweight.

---

# 13. Phase 9 — Server-Side HTML Templates

Static HTML should be the primary way of building websites.

Templates are an optional feature for when developers need server-generated content.

Example:

```text
templates/
├── index.html
└── profile.html
```

Template:

```html
<h1>Welcome, {{ name }}</h1>
```

C++ provides the data:

```text
name = "Motheo"
```

Forge produces:

```html
<h1>Welcome, Motheo</h1>
```

This is server-side rendering.

The template system should remain deliberately simple.

### Tasks

- [ ] Support raw HTML responses
- [ ] Create template loader
- [ ] Create basic variable substitution
- [ ] Add template directory configuration
- [ ] Add template rendering API
- [ ] Escape user-provided HTML where appropriate

---

# 14. Phase 10 — Application Project Structure

Make Forge applications easy to organise.

Example:

```text
my-forge-app/
│
├── CMakeLists.txt
│
├── src/
│   ├── main.cpp
│   ├── routes.cpp
│   └── controllers/
│
├── public/
│   ├── index.html
│   ├── css/
│   ├── js/
│   └── images/
│
├── templates/
│   ├── layout.html
│   └── pages/
│
└── config/
```

Forge itself should remain separate:

```text
ForgeHTTP/
├── include/
├── src/
├── tests/
└── examples/
```

Applications should depend on Forge rather than modifying Forge's source code.

---

# 15. Phase 11 — Example Applications

Once the framework features are stable, create small example applications.

These examples are an important part of the project.

## Example 1 — Hello World

```text
GET /
```

Returns a simple HTML page.

Purpose:

Demonstrate the basic framework API.

---

## Example 2 — Personal Website

Pages:

```text
/
 /about
 /projects
 /contact
```

Frontend:

```text
HTML
CSS
JavaScript
Images
```

Backend:

```text
C++
Forge
```

Features:

- Static HTML
- CSS
- Images
- Navigation
- Contact form
- C++ backend endpoint

Purpose:

Demonstrate that Forge can build a real small website.

---

## Example 3 — Blog

Features:

- Posts
- Dynamic routes
- Templates
- Query parameters
- Simple data storage

Example:

```text
/blog
/blog/hello-world
/blog/my-second-post
```

Purpose:

Demonstrate server-side rendering and routing.

---

## Example 4 — REST API

Endpoints:

```text
GET    /api/users
GET    /api/users/:id
POST   /api/users
DELETE /api/users/:id
```

Purpose:

Demonstrate API development.

---

## Example 5 — Interactive Dashboard

Frontend:

```text
HTML
CSS
JavaScript
```

Backend:

```text
C++
Forge
```

The JavaScript frontend communicates with Forge through JSON APIs.

Purpose:

Demonstrate:

```text
Browser
   ↓
JavaScript fetch()
   ↓
Forge
   ↓
C++ application logic
   ↓
JSON
   ↓
Browser
```

---

# 16. Phase 12 — Testing

Every major framework feature should have tests.

Test:

- [ ] Route registration
- [ ] Route matching
- [ ] Route parameters
- [ ] Query parameters
- [ ] Request parsing
- [ ] Response generation
- [ ] Headers
- [ ] Static files
- [ ] Middleware
- [ ] 404 responses
- [ ] 405 responses
- [ ] JSON responses
- [ ] Template rendering
- [ ] Path traversal protection
- [ ] Frontend/API integration

Also create integration tests that start Forge and send real HTTP requests.

---

# 17. Phase 13 — Documentation

Forge should eventually have documentation that allows someone unfamiliar with the source code to build an application.

Documentation should include:

```text
Getting Started
Installation
Creating an Application
Project Structure
Routing
Requests
Responses
Static Files
HTML/CSS/JavaScript
Frontend/API Communication
Middleware
Templates
JSON APIs
Examples
Architecture
```

The first example should show a complete small website:

```text
C++ → Forge
       │
       ├── routes
       ├── API
       └── static files
              │
              ├── HTML
              ├── CSS
              └── JavaScript
```

---

# 18. Final Architecture

The long-term architecture should look approximately like:

```text
                  Forge Application
                         │
          ┌──────────────┴──────────────┐
          │                             │
       C++ Code                    Web Frontend
          │                             │
       Forge API                  HTML / CSS / JS
          │                             │
          └──────────────┬──────────────┘
                         │
                         ▼
                       Browser
```

Underneath:

```text
                     Forge
                       │
                ┌──────┴──────┐
                │             │
             Router       Middleware
                │             │
                └──────┬──────┘
                       ▼
                    Request
                       │
                       ▼
                 C++ Handler
                       │
                       ▼
                    Response
                       │
             ┌─────────┴─────────┐
             ▼                   ▼
          HTML/Files            JSON
             │                   │
             └─────────┬─────────┘
                       ▼
                 HTTP Server
                       │
                       ▼
                     Socket
```

---

# 19. What Developers Actually Write

A Forge developer should primarily work with **three technologies**:

### HTML

Used to define the structure of the website.

```text
<h1>
<p>
<nav>
<section>
<form>
<button>
```

### CSS

Used to style the website.

```text
layout
spacing
typography
animations
responsive design
```

### JavaScript

Used for browser-side interactivity.

```text
buttons
forms
animations
fetch()
API calls
DOM manipulation
```

### C++

Used for server-side functionality.

```text
routing
business logic
authentication
database access
API endpoints
file handling
middleware
```

Forge connects these technologies.

---

# 20. What Forge Should NOT Become

Avoid adding features simply because large frameworks have them.

Do not initially implement:

- Full ORM
- Authentication platform
- Complex dependency injection
- Full MVC architecture
- WebSockets
- HTTP/2
- HTTP/3
- Distributed systems
- Production-grade load balancing
- Complex template languages
- Automatic code generation
- Large third-party dependency trees
- A custom HTML language
- A custom CSS/JavaScript replacement

Forge should **use existing web standards rather than reinvent them**.

HTML should remain HTML.

CSS should remain CSS.

JavaScript should remain JavaScript.

C++ should provide the server-side functionality.

---

# 21. Definition of Done

Forge can be considered a simple web framework when a developer can:

1. Create a new C++ application.
2. Include Forge.
3. Start an HTTP server.
4. Register routes.
5. Access request information.
6. Generate HTML responses.
7. Serve normal HTML/CSS/JavaScript files.
8. Serve images and other static assets.
9. Communicate with C++ through JSON APIs.
10. Use dynamic route parameters.
11. Use middleware.
12. Render simple templates.
13. Build a small interactive website.
14. Build and run the website without touching Forge's networking implementation.

The developer writes:

```text
HTML
CSS
JavaScript
C++
```

Forge handles:

```text
Sockets
HTTP
Routing
Middleware
Static files
Requests
Responses
Server lifecycle
```

---

# 22. Recommended Development Order

Do not implement everything simultaneously.

Follow this order:

```text
1. Stabilise HTTP server
        ↓
2. Application class
        ↓
3. Router
        ↓
4. Request / Response API
        ↓
5. Static files
        ↓
6. HTML/CSS/JS website support
        ↓
7. Frontend ↔ C++ API communication
        ↓
8. Middleware
        ↓
9. JSON
        ↓
10. Route parameters
        ↓
11. Templates
        ↓
12. Example applications
        ↓
13. Testing
        ↓
14. Documentation
```

Each phase should produce something usable before moving to the next.

---

# 23. Core Principle

ForgeHTTP should demonstrate that I can understand and implement the layers beneath a web framework while still designing a usable developer-facing API.

The framework should **not attempt to replace web technologies**.

Instead:

```text
HTML + CSS + JavaScript
          │
          │ frontend
          ▼
        Browser
          │
          │ HTTP
          ▼
         Forge
          │
          │ C++
          ▼
      Application
```

The developer writes the **website and application**.

Forge provides the **HTTP infrastructure and framework abstractions**.

The final goal is a small C++ web framework that can be used to build genuine small websites and APIs while remaining simple enough to understand from the source code.