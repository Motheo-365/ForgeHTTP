import { useEffect } from "react";
import { useLocation } from "react-router-dom";

import Terminal from "./terminal";

import "../styles/introduction.css";

const pipeline = [
    ["01", "CLIENT", "HTTP request"],
    ["02", "TCP SOCKET", "Accept connection"],
    ["03", "HTTP PARSER", "Parse request"],
    ["04", "THREAD POOL", "Process concurrently"],
    ["05", "MIDDLEWARE", "Process request"],
    ["06", "ROUTER", "Match endpoint"],
    ["07", "HANDLER", "Generate result"],
    ["08", "HTTP RESPONSE", "Send to client"]
];

function Introduction() {
    const location = useLocation();

    useEffect(() => {
        if (!location.hash) {
            return;
        }

        const target = document.getElementById(location.hash.slice(1));

        if (target) {
            requestAnimationFrame(() => {
                target.scrollIntoView({
                    behavior: "smooth",
                    block: "start"
                });
            });
        }
    }, [location.hash]);

    return (
        <div className="introduction">
            <section className="intro-hero">
                <div className="intro-eyebrow">FORGEHTTP / SERVER ADMINISTRATION</div>
                <h1>ForgeHTTP</h1>
                <p className="intro-subtitle">
                    LIGHTWEIGHT HTTP/1.1 SERVER<br />
                    BUILT FROM SCRATCH IN C++20
                </p>
                <p className="intro-description">
                    ForgeHTTP is a lightweight HTTP/1.1 server and web
                    application framework built from the ground up using
                    C++20 and POSIX sockets.
                </p>
                <div className="intro-terminal">
                    <span>forgehttp@server:~$</span>
                    <span className="terminal-cursor">_</span>
                    <a href="#terminal">click here</a>
                </div>
            </section>

            <section id="overview" className="intro-section">
                <div className="section-label">01 / OVERVIEW</div>
                <h2>What is ForgeHTTP?</h2>
                <p>
                    ForgeHTTP is a custom HTTP server designed to demonstrate
                    how a web server works beneath higher-level frameworks.
                    It implements networking, request processing, middleware,
                    routing, and responses in C++20.
                </p>
                <div className="tech-grid">
                    {[
                        ["C++20", "Core implementation language"],
                        ["POSIX SOCKETS", "TCP network communication"],
                        ["HTTP/1.1", "HTTP request and response handling"],
                        ["THREAD POOL", "Concurrent request processing"],
                        ["MIDDLEWARE", "Request processing pipeline"],
                        ["ROUTING", "HTTP method and path matching"]
                    ].map(([name, description]) => (
                        <div className="tech-item" key={name}>
                            <span className="tech-name">{name}</span>
                            <span className="tech-description">{description}</span>
                        </div>
                    ))}
                </div>
            </section>

            <section id="request-lifecycle" className="intro-section">
                <div className="section-label">02 / REQUEST LIFECYCLE</div>
                <h2>How ForgeHTTP works</h2>
                <p>
                    Every request travels through a series of stages before
                    ForgeHTTP sends a response back to the client.
                </p>
                <div className="pipeline">
                    {pipeline.map(([number, name, description], index) => (
                        <div key={number}>
                            <div className="pipeline-step">
                                <span className="pipeline-number">{number}</span>
                                <strong>{name}</strong>
                                <span>{description}</span>
                            </div>
                            {index < pipeline.length - 1 && <div className="pipeline-arrow">v</div>}
                        </div>
                    ))}
                </div>
            </section>

            <section id="dashboard" className="intro-section">
                <div className="section-label">03 / DASHBOARD</div>
                <h2>Using the dashboard</h2>
                <p>
                    Use the dashboard to observe the server and work through
                    its main operational views.
                </p>
                <div className="dashboard-list">
                    {[
                        ["INTRODUCTION", "Learn about ForgeHTTP and the dashboard."],
                        ["DASHBOARD", "View overall server state and activity."],
                        ["REQUESTS", "Inspect HTTP request activity."],
                        ["ROUTES", "Explore registered HTTP routes."],
                        ["MIDDLEWARE", "Visualise the request pipeline."],
                        ["METRICS", "Monitor performance and activity."],
                        ["HEALTH", "Check whether the server is responding."],
                        ["CONFIGURATION", "View and update server settings."],
                        ["TERMINAL", "Run ForgeHTTP commands and inspect the server."]
                    ].map(([name, description]) => (
                        <div className="dashboard-item" key={name}>
                            <span className="dashboard-command">&gt; {name}</span>
                            <span>{description}</span>
                        </div>
                    ))}
                </div>
            </section>

            <section id="server-guide" className="intro-section">
                <div className="section-label">04 / SERVER GUIDE</div>
                <h2>Using the server</h2>
                <p>
                    ForgeHTTP uses two different terminals. Use your PC terminal
                    to download, build, and start the project. Once the dashboard
                    is running, use the ForgeHTTP Terminal to inspect the server.
                </p>

                <div className="server-guide-grid">
                    <div className="server-guide-divider" role="separator">
                        <span>PC TERMINAL INSTRUCTIONS</span>
                    </div>
                    <div className="server-guide-step">
                        <span className="server-guide-number">01</span>
                        <div>
                            <span className="page-label">PC TERMINAL</span>
                            <h3>Clone ForgeHTTP</h3>
                            <p>Clone the project from GitHub and enter the project directory.</p>
                            <pre><code>{"git clone https://github.com/Motheo-365/ForgeHTTP.git\ncd ForgeHTTP"}</code></pre>
                        </div>
                    </div>

                    <div className="server-guide-step">
                        <span className="server-guide-number">02</span>
                        <div>
                            <span className="page-label">PC TERMINAL</span>
                            <h3>Build and start the server</h3>
                            <p>Build the C++20 backend and start the ForgeHTTP server.</p>
                            <pre><code>{"cmake -S backend -B backend/build\ncmake --build backend/build\n./backend/build/forge"}</code></pre>
                        </div>
                    </div>

                    <div className="server-guide-step">
                        <span className="server-guide-number">03</span>
                        <div>
                            <span className="page-label">PC TERMINAL</span>
                            <h3>Start the dashboard</h3>
                            <p>Open a second PC terminal, start the React client, and open the Vite development URL.</p>
                            <pre><code>{"cd ForgeHTTP/client\nnpm install\nnpm run dev"}</code></pre>
                        </div>
                    </div>

                    <div className="server-guide-divider" role="separator">
                        <span>FORGEHTTP TERMINAL INSTRUCTIONS</span>
                    </div>

                    <div className="server-guide-step">
                        <span className="server-guide-number">04</span>
                        <div>
                            <span className="page-label">FORGEHTTP TERMINAL</span>
                            <h3>Check server health</h3>
                            <p>Open the ForgeHTTP Terminal and verify that the server is responding.</p>
                            <pre><code>health</code></pre>
                        </div>
                    </div>

                    <div className="server-guide-step">
                        <span className="server-guide-number">05</span>
                        <div>
                            <span className="page-label">FORGEHTTP TERMINAL</span>
                            <h3>Inspect server activity</h3>
                            <p>View request counts, active connections, response time, errors, and worker information.</p>
                            <pre><code>metrics</code></pre>
                        </div>
                    </div>

                    <div className="server-guide-step">
                        <span className="server-guide-number">06</span>
                        <div>
                            <span className="page-label">FORGEHTTP TERMINAL</span>
                            <h3>Inspect requests and routes</h3>
                            <p>View recent HTTP requests and the routes registered with ForgeHTTP.</p>
                            <pre><code>{"requests\nroutes"}</code></pre>
                        </div>
                    </div>

                    <div className="server-guide-step">
                        <span className="server-guide-number">07</span>
                        <div>
                            <span className="page-label">FORGEHTTP TERMINAL</span>
                            <h3>Inspect configuration</h3>
                            <p>View the current server configuration and its middleware settings.</p>
                            <pre><code>config</code></pre>
                        </div>
                    </div>

                    <div className="server-guide-step">
                        <span className="server-guide-number">08</span>
                        <div>
                            <span className="page-label">FORGEHTTP TERMINAL</span>
                            <h3>View available commands</h3>
                            <p>Use the built-in help command whenever you need to see the available ForgeHTTP commands.</p>
                            <pre><code>help</code></pre>
                        </div>
                    </div>
                </div>
            </section>

            <section id="terminal" className="intro-section terminal">
                <div className="section-label">05 / TERMINAL</div>
                <Terminal />
            </section>

            <section id="quick-start" className="intro-section intro-quick-start">
                <div className="section-label">06 / QUICK START</div>
                <h2>Get started</h2>
                <div className="quick-start">
                    <div><span>01</span>Clone ForgeHTTP.</div>
                    <div><span>02</span>Build and start the server.</div>
                    <div><span>03</span>Start the React dashboard.</div>
                    <div><span>04</span>Run <code>health</code> in Terminal.</div>
                    <div><span>05</span>Inspect <code>metrics</code> and <code>requests</code>.</div>
                    <div><span>06</span>Inspect <code>routes</code> and <code>config</code>.</div>
                </div>
            </section>
        </div>
    );
}

export default Introduction;