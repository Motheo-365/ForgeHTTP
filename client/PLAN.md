# ForgeHTTP — Frontend Project Plan

The ForgeHTTP frontend is a React-based administration dashboard for the ForgeHTTP C++ HTTP/1.1 server.

The frontend is designed as an **old-school server administration website** inspired by Unix systems, terminal interfaces, and classic green-on-black computer displays. The visual style is intentionally retro, while the underlying interface follows modern principles of usability, accessibility, responsiveness, and information hierarchy.

The dashboard consumes ForgeHTTP's own HTTP API and provides a visual way to understand, monitor, and interact with the server.

**Goal:** Build a polished, practical administration dashboard that makes the engineering behind ForgeHTTP visible while demonstrating strong React, API integration, and frontend design skills.

---

## 1. Vision & Success Criteria

The ForgeHTTP dashboard should feel like a graphical control panel for a real HTTP server.

It should communicate:

- What ForgeHTTP is
- How the server works
- How to use the dashboard
- Whether the server is running
- What requests the server is handling
- Which routes are registered
- How middleware processes requests
- How the server is performing

The visual design should feel like an old-school server interface without becoming difficult to use.

### Design Philosophy

> **Retro appearance, modern usability.**

The dashboard should use the visual language of:

- Unix administration tools
- CRT terminals
- Classic server consoles
- Early computer interfaces
- Green-on-black displays

But it should still provide:

- Clear navigation
- Strong information hierarchy
- Readable typography
- Responsive layouts
- Accessible controls
- Clear loading and error states
- Consistent spacing
- Predictable interactions

### Definition of Done

The frontend is complete when:

- The React application runs independently from the C++ server.
- The Introduction page explains ForgeHTTP and how to use the dashboard.
- The Dashboard displays live server information.
- `/health` is consumed by the frontend.
- `/metrics` is consumed by the frontend.
- Server status is clearly displayed.
- Recent request activity can be displayed.
- Registered routes can be displayed.
- The middleware pipeline can be visualised.
- Metrics can be visualised using charts.
- API failures are handled gracefully.
- The interface is responsive.
- The interface follows the established visual system.
- The frontend can be built and deployed.

---

# 2. Tech Stack

| Concern | Choice | Why |
|---|---|---|
| Framework | React | Component-based frontend architecture |
| Build Tool | Vite | Lightweight and fast development environment |
| Language | JavaScript / JSX | Simple and familiar for the dashboard |
| Styling | CSS | Full control over the custom visual design |
| API Communication | Fetch | Avoid unnecessary HTTP abstraction |
| Charts | Recharts | React-compatible metrics visualisation |
| Routing | React Router | Client-side dashboard navigation |
| Package Manager | npm | Standard React tooling |
| Backend | ForgeHTTP C++ API | Dashboard consumes the server's own API |
| Deployment | Docker | Consistent deployment |

Dependencies should remain deliberately minimal.

The frontend should demonstrate React development and frontend engineering rather than relying on a large UI framework.

---

# 3. Visual Design System

## 3.1 Overall Style

The dashboard uses an **old-school green and black server aesthetic**.

The design should resemble a modern interpretation of a classic server console.

Important characteristics:

- Near-black backgrounds
- Green primary accents
- Muted green secondary colours
- Thin borders
- Monospace typography
- Minimal rounded corners
- Dense information presentation
- Clear spacing
- Simple geometric layouts
- Subtle terminal-inspired details

The interface should not look like a generic modern SaaS dashboard.

Avoid:

- Large colourful gradients
- Excessive rounded cards
- Purple/blue startup aesthetics
- Excessive glassmorphism
- Decorative animations
- Excessive neon green
- Fake terminal interactions

---

## 3.2 Colour System

Initial colour system:

| Role | Colour |
|---|---|
| Background | `#0A0D0A` |
| Panel | `#0F140F` |
| Border | `#263326` |
| Primary Green | `#33FF66` |
| Secondary Green | `#79A87F` |
| Main Text | `#D8E6D8` |
| Muted Text | `#718071` |
| Success | `#33FF66` |
| Warning | `#D6C85A` |
| Error | `#FF5C5C` |

Bright green should be reserved for important information such as:

- Active navigation
- Server status
- Successful requests
- Important metrics
- Interactive highlights
- Selected elements

Normal body text should use softer green/grey tones for readability.

---

# 4. Typography

The dashboard should use a monospace-first typography system.

Preferred fonts:

- JetBrains Mono
- IBM Plex Mono

Typography should reinforce the server/terminal aesthetic without compromising readability.

### Example hierarchy

```text
FORGEHTTP

SERVER OVERVIEW

12,452

GET /api/users

Server is operating normally.
```

Large metrics should be visually prominent.

Technical information such as:

- HTTP methods
- Routes
- Status codes
- IP addresses
- Server configuration
- Request information

should use the monospace typeface consistently.

---

# 5. Application Layout

The application uses a persistent administration layout.

```text
┌──────────────────────────────────────────────────────────────┐
│ FORGEHTTP                              ● ONLINE               │
├────────────────┬─────────────────────────────────────────────┤
│                │                                             │
│ GETTING        │                                             │
│ STARTED        │                                             │
│                │                                             │
│ > Introduction │              PAGE CONTENT                   │
│   Dashboard    │                                             │
│                │                                             │
│ SERVER         │                                             │
│                │                                             │
│   Requests     │                                             │
│   Routes       │                                             │
│   Middleware   │                                             │
│   Metrics      │                                             │
│   Health       │                                             │
│   Configuration│                                             │
│                │                                             │
└────────────────┴─────────────────────────────────────────────┘
```

### Global Components

- Application shell
- Sidebar
- Header
- Main content area
- Page container
- Server status indicator

---

# 6. Navigation

Navigation should be divided into logical sections.

```text
FORGEHTTP

GETTING STARTED

> Introduction
  Dashboard

SERVER

  Requests
  Routes
  Middleware
  Metrics
  Health
  Configuration
```

The active page should be clearly identified.

Example:

```text
> Dashboard
```

Inactive pages:

```text
  Requests
  Routes
  Metrics
```

The `>` symbol reinforces the terminal-inspired aesthetic without requiring the application to function as an actual terminal.

---

# 7. Frontend Architecture

The frontend should be divided into:

```text
client/

├── src/
│   ├── components/
│   │   ├── Layout/
│   │   ├── Sidebar/
│   │   ├── Header/
│   │   ├── StatCard/
│   │   ├── StatusIndicator/
│   │   ├── RequestTable/
│   │   ├── MetricChart/
│   │   ├── RouteTable/
│   │   └── MiddlewarePipeline/
│   │
│   ├── pages/
│   │   ├── Introduction.tsx
│   │   ├── Dashboard.tsx
│   │   ├── Requests.tsx
│   │   ├── Routes.tsx
│   │   ├── Middleware.tsx
│   │   ├── Metrics.tsx
│   │   ├── Health.tsx
│   │   └── Configuration.tsx
│   │
│   ├── services/
│   │   └── api.ts
│   │
│   ├── App.tsx
│   ├── main.tsx
│   └── index.css
│
├── public/
├── package.json
└── vite.config.js
```

Components should be created when they provide:

- Reusability
- Clear responsibility
- Independent behaviour
- Improved code organisation

Avoid creating unnecessary components for trivial elements.

---

# 8. API Integration

The React dashboard communicates directly with ForgeHTTP.

```text
React Dashboard
       │
       │ HTTP
       ▼
ForgeHTTP :8086
       │
       ├── /health
       ├── /metrics
       ├── /api/users
       └── other API endpoints
```

There should be **no separate Express backend** for the dashboard.

ForgeHTTP itself is the backend.

---

## 8.1 API Service

API communication should be centralised in:

```text
src/services/api.ts
```

Potential functions:

```text
getHealth()
getMetrics()
getUsers()
```

Additional API functions can be added as ForgeHTTP exposes more endpoints.

The purpose of the service layer is to prevent individual React components from duplicating request logic.

---

# 9. Introduction Page

The **Introduction page is the landing page of the dashboard**.

Its purpose is to explain ForgeHTTP before the user begins monitoring the server.

It should answer:

- What is ForgeHTTP?
- Why was it built?
- What technologies does it use?
- How does the server work?
- What does the dashboard show?
- How do I use the dashboard?
- What should I do if the server is offline?

---

## 9.1 Introduction Hero

The top of the page should establish the project immediately.

Example:

```text
FORGEHTTP

LIGHTWEIGHT HTTP/1.1 SERVER
BUILT FROM SCRATCH IN C++20

ForgeHTTP is a lightweight HTTP/1.1 server
and web application framework built from
the ground up using C++20 and POSIX sockets.

● SERVER ONLINE
```

The page can include a small terminal-inspired prompt:

```text
forgehttp@server:~$ _
```

This should be a visual detail rather than an actual command interface.

---

## 9.2 What Is ForgeHTTP?

Explain the project in accessible language.

Topics:

- C++20
- POSIX sockets
- HTTP/1.1
- Request parsing
- Routing
- Middleware
- Thread pool
- JSON APIs
- Observability

The section should explain that ForgeHTTP was built from the networking layer upward rather than using an existing HTTP server framework.

---

## 9.3 How ForgeHTTP Works

Display the request lifecycle visually:

```text
CLIENT
   │
   ▼
TCP SOCKET
   │
   ▼
HTTP PARSER
   │
   ▼
THREAD POOL
   │
   ▼
MIDDLEWARE
   │
   ▼
ROUTER
   │
   ▼
HANDLER
   │
   ▼
HTTP RESPONSE
   │
   ▼
CLIENT
```

Each stage can have a short explanation.

---

## 9.4 Using the Dashboard

Explain what each dashboard page does.

```text
INTRODUCTION
Learn about ForgeHTTP and the dashboard.

DASHBOARD
View the overall state of the server.

REQUESTS
Inspect HTTP request activity.

ROUTES
View registered HTTP routes.

MIDDLEWARE
Explore the middleware request pipeline.

METRICS
Monitor server performance.

HEALTH
Check whether the server is responding.

CONFIGURATION
View server configuration.
```

---

## 9.5 Quick Start

Provide a simple usage guide:

```text
1. Start ForgeHTTP.

2. Start the React dashboard.

3. Check the server status.

4. Open Dashboard.

5. Explore Requests and Metrics.

6. Inspect Routes and Middleware.
```

The Introduction page should be useful even to someone who has never seen the project before.

---

# 10. Dashboard Page

The Dashboard provides the high-level server overview.

### Statistics

Display:

- Total requests
- Active connections
- Average response time
- Error count
- Worker count
- Server uptime
- Requests per second

Example:

```text
SERVER OVERVIEW

┌──────────────┐ ┌──────────────┐ ┌──────────────┐
│ REQUESTS     │ │ ACTIVE CONNS │ │ AVG LATENCY  │
│ 12,452       │ │ 17           │ │ 8.4 ms       │
└──────────────┘ └──────────────┘ └──────────────┘

┌──────────────┐ ┌──────────────┐
│ ERRORS       │ │ WORKERS      │
│ 31           │ │ 8            │
└──────────────┘ └──────────────┘
```

---

## 10.1 Request Activity

Display a requests-per-second or requests-over-time chart.

```text
REQUEST ACTIVITY

┌─────────────────────────────────────────────────┐
│                  /\                             │
│       /\       /    \      /\                   │
│  ____/  \_____/      \____/  \____              │
└─────────────────────────────────────────────────┘
```

The chart should use real `/metrics` data.

---

## 10.2 Recent Requests

Display a compact request table:

```text
RECENT REQUESTS

GET    /health       200    2ms
GET    /api/users    200    4ms
POST   /api/users    201    8ms
GET    /unknown      404    1ms
```

---

# 11. Requests Page

The Requests page provides a detailed view of HTTP activity.

### Table

| Time | Method | Path | Status | Latency |
|---|---|---|---|---|
| 12:41:03 | GET | `/health` | 200 | 2ms |
| 12:41:04 | GET | `/api/users` | 200 | 4ms |
| 12:41:06 | POST | `/api/users` | 201 | 8ms |
| 12:41:08 | GET | `/unknown` | 404 | 1ms |

HTTP methods and status codes should have clear visual treatment.

### Future Features

Optional:

- Method filtering
- Status filtering
- Route searching
- Request details
- Live request stream

These features should not block the initial implementation.

---

# 12. Routes Page

The Routes page displays registered ForgeHTTP routes.

Example:

```text
REGISTERED ROUTES

METHOD       PATH

GET          /health
GET          /api/users
POST         /api/users
GET          /metrics
```

The page should make the router architecture visible without requiring the user to inspect the C++ source code.

### Future Route Information

If the backend exposes route metadata, display:

- HTTP method
- Path
- Parameters
- Middleware
- Handler information

---

# 13. Middleware Page

The Middleware page visualises ForgeHTTP's Chain of Responsibility.

```text
MIDDLEWARE PIPELINE

┌─────────────────┐
│      CORS       │
└────────┬────────┘
         ↓
┌─────────────────┐
│      LOGGER     │
└────────┬────────┘
         ↓
┌─────────────────┐
│   RATE LIMITER  │
└────────┬────────┘
         ↓
┌─────────────────┐
│      AUTH       │
└────────┬────────┘
         ↓
       ROUTER
```

Each middleware should display:

- Name
- Status
- Short description

Example:

```text
CORS

● ACTIVE

Handles cross-origin requests
and OPTIONS preflight requests.
```

The purpose of this page is primarily to communicate the architecture of ForgeHTTP.

---

# 14. Metrics Page

The Metrics page provides detailed server performance information.

### Metrics

- Total requests
- Requests per second
- Active connections
- Average response time
- Error count
- Worker count

### Charts

Use Recharts for:

- Requests over time
- Response time over time
- Error activity

The initial implementation should use polling.

```text
React
  │
  │ GET /metrics
  ▼
ForgeHTTP
  │
  ▼
JSON Metrics
  │
  ▼
React State
  │
  ▼
Chart
```

A WebSocket-based live metrics system may be considered later.

---

# 15. Health Page

The Health page provides a simple server health overview.

Example:

```text
SYSTEM HEALTH

SERVER

● ONLINE

HTTP
● RESPONDING

NETWORK
● LISTENING

THREAD POOL
● 8 WORKERS

METRICS
● AVAILABLE
```

The `/health` endpoint should be the primary source for server availability.

### Health States

```text
● CHECKING
● ONLINE
● OFFLINE
● ERROR
```

---

# 16. Configuration Page

The Configuration page displays server configuration information.

Potential information:

```text
SERVER CONFIGURATION

Host              0.0.0.0
Port              8086
HTTP Version      HTTP/1.1
Workers           8
Rate Limit        Enabled
CORS              Enabled
Authentication    Enabled
```

The initial implementation should be **read-only**.

Configuration editing should only be introduced if ForgeHTTP later exposes a safe configuration API.

---

# 17. Global Server Status

The server status should always be visible in the application header.

Example:

```text
FORGEHTTP                         ● ONLINE
```

The status should be determined using `/health`.

The frontend should never simply assume that the backend is available.

### States

```text
Loading
● CHECKING

Online
● ONLINE

Offline
● OFFLINE

Error
● ERROR
```

Status should be communicated through both colour and text.

---

# 18. Loading & Error States

Every API-driven page should handle:

- Loading
- Success
- Empty data
- Server unavailable
- API errors

### Loading

```text
LOADING METRICS...
```

### Server Offline

```text
FORGEHTTP UNAVAILABLE

Unable to connect to the server.

Check that ForgeHTTP is running.
```

### API Error

```text
REQUEST FAILED

Unable to retrieve metrics.

[ RETRY ]
```

The interface should never appear broken or completely empty when the API is unavailable.

---

# 19. Responsive Design

The dashboard should primarily target desktop administration use while remaining usable on smaller screens.

Supported layouts:

- Desktop
- Laptop
- Tablet
- Mobile

### Desktop

```text
Sidebar + Main Content
```

### Mobile

```text
Header
Navigation
Main Content
```

The sidebar should collapse into a mobile navigation interface.

Tables may use horizontal scrolling or selectively hide less-important columns.

---

# 20. Animation & Effects

Animation should be subtle and functional.

Possible uses:

- Blinking terminal cursor
- Status transitions
- Chart updates
- Navigation transitions
- Loading indicators

Avoid excessive animation.

A subtle CRT or scanline effect may be used as a background detail.

It must never interfere with:

- Text readability
- Contrast
- Navigation
- Charts
- Accessibility

The retro effect should support the design rather than dominate it.

---

# 21. Accessibility

The visual style must not compromise accessibility.

The dashboard should provide:

- Strong colour contrast
- Keyboard navigation
- Visible focus states
- Semantic HTML
- Descriptive labels
- Accessible buttons
- Screen-reader-friendly status indicators
- Information that does not rely on colour alone

For example:

```text
● ONLINE
```

should communicate the state through the text as well as the green colour.

---

# 22. Development Workflow

Create the frontend from the repository root:

```bash
npm create vite@latest client
```

Install dependencies:

```bash
cd client
npm install
```

Install frontend dependencies:

```bash
npm install react-router-dom recharts
```

Run the development server:

```bash
npm run dev
```

The backend should run separately.

### Terminal 1

```bash
PORT=8086 ./backend/build/forge
```

### Terminal 2

```bash
cd client
npm run dev
```

Expected architecture:

```text
┌────────────────────┐
│ React Development  │
│ Server             │
└─────────┬──────────┘
          │
          │ HTTP
          ▼
┌────────────────────┐
│ ForgeHTTP :8086    │
│ C++ HTTP Server    │
└────────────────────┘
```

---

# 23. Frontend Milestones

| # | Milestone | Outcome |
|---|---|---|
| 1 | React/Vite setup | Basic frontend application |
| 2 | Design system | Green/black visual system and typography |
| 3 | Application shell | Header, sidebar, navigation, content area |
| 4 | Introduction | Project explanation and dashboard onboarding |
| 5 | API service | Centralised ForgeHTTP API communication |
| 6 | Dashboard | Server overview and statistics |
| 7 | Metrics | `/metrics` integration and charts |
| 8 | Requests | Request activity interface |
| 9 | Routes | Registered route visualisation |
| 10 | Middleware | Middleware pipeline visualisation |
| 11 | Health | Server health monitoring |
| 12 | Configuration | Server configuration display |
| 13 | Error handling | Loading, offline, and API error states |
| 14 | Responsive design | Tablet and mobile layouts |
| 15 | Accessibility | Keyboard, focus, contrast, semantic HTML |
| 16 | Visual polish | CRT details, animations, spacing, refinement |
| 17 | Docker | Containerised frontend |
| 18 | Deployment | Full ForgeHTTP + dashboard deployment |

---

# 24. Immediate Implementation Order

Build the frontend in the following order.

## Step 1 — Create React Application

Create:

```text
client/
```

using Vite and React.

---

## Step 2 — Establish Design System

Implement:

- CSS variables
- Colours
- Typography
- Global spacing
- Borders
- Background
- Buttons
- Status indicators

The design system should be established before building individual pages.

---

## Step 3 — Build Application Shell

Create:

```text
App
 └── Layout
      ├── Sidebar
      ├── Header
      └── Main Content
```

Implement navigation with React Router.

---

## Step 4 — Build Introduction Page

Create the landing page explaining:

- What ForgeHTTP is
- How it works
- Why it exists
- What each dashboard page does
- How to start using the dashboard

This page should work even before live metrics are connected.

---

## Step 5 — Build API Service

Create:

```text
src/services/api.ts
```

and implement communication with:

```text
GET /health
GET /metrics
GET /api/users
```

---

## Step 6 — Build Dashboard

Create the main overview page.

Initially establish the layout and components, then connect real backend data.

---

## Step 7 — Connect Live Data

Replace placeholder values with actual ForgeHTTP API responses.

The dashboard should now display real server information.

---

## Step 8 — Build Remaining Pages

Implement:

```text
Requests
Routes
Middleware
Metrics
Health
Configuration
```

---

## Step 9 — Responsive & Accessibility Pass

Test:

- Different screen sizes
- Keyboard navigation
- Focus states
- Colour contrast
- API failure states

---

## Step 10 — Visual Polish

Add:

- Subtle CRT details
- Terminal cursor
- Status animations
- Chart styling
- Hover states
- Focus states
- Empty states
- Error states

Effects should remain restrained.

---

## Step 11 — Docker & Deployment

Containerise the React frontend and integrate it with the ForgeHTTP deployment.

The final deployment should allow the dashboard and C++ server to operate together.

---

# 25. Finished Frontend Architecture

The finished frontend should provide:

```text
                         FORGEHTTP
                            │
              ┌─────────────┴─────────────┐
              │                           │
         Introduction                 Dashboard
              │                           │
              │                    ┌──────┴──────┐
              │                    │             │
              │                 Metrics       Health
              │                    │
              │                 Requests
              │                    │
              │                  Routes
              │                    │
              │               Middleware
              │                    │
              │              Configuration
              │
              └──────────────┬──────────────┘
                             │
                             ▼
                      ForgeHTTP API
                             │
                             ▼
                     C++ HTTP Server
```

The frontend should make the underlying C++ engineering visible rather than hiding it behind a generic interface.

---

# 26. Final Product Experience

A user opening ForgeHTTP should experience the application in this order:

```text
OPEN FORGEHTTP
      │
      ▼
INTRODUCTION
"What is this?"
      │
      ▼
DASHBOARD
"What is the server doing?"
      │
      ▼
REQUESTS
"What requests is it handling?"
      │
      ▼
ROUTES
"What can the server handle?"
      │
      ▼
MIDDLEWARE
"How are requests processed?"
      │
      ▼
METRICS
"How is the server performing?"
      │
      ▼
HEALTH
"Is everything working?"
```

The Introduction page provides context.

The Dashboard provides the overview.

The remaining pages expose the underlying architecture and runtime behaviour.

---

# 27. Project Rule

The frontend should follow the same philosophy as the backend:

> **Don't add complexity unless it solves a real problem.**

Avoid:

- Unnecessary state-management libraries
- Large UI component frameworks
- Excessive dependencies
- Over-engineered API abstractions
- Fake terminal functionality
- Decorative features with no purpose
- Excessive animations
- Visual effects that compromise readability

React, CSS, Fetch, React Router, and Recharts are sufficient.

The frontend exists to demonstrate:

- Practical React development
- Component architecture
- API integration
- Data visualisation
- Responsive UI design
- Accessibility
- Server monitoring
- The capabilities of ForgeHTTP itself

The final product should feel like a **real server administration interface built for ForgeHTTP**, not simply a React dashboard attached to a C++ project.