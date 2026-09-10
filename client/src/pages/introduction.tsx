import { useEffect } from "react";
import { useLocation } from "react-router-dom";

import "../styles/introduction.css";

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
                <div className="intro-eyebrow">
                    FORGEHTTP / SERVER ADMINISTRATION
                </div>

                <h1>ForgeHTTP</h1>

                <p className="intro-subtitle">
                    LIGHTWEIGHT HTTP/1.1 SERVER
                    <br />
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
                </div>
            </section>

                <section id="overview" className="intro-section">
                <div className="section-label">01 / OVERVIEW</div>

                <h2>What is ForgeHTTP?</h2>

                <p>
                    ForgeHTTP is a custom HTTP server designed to demonstrate
                    how a web server works beneath higher-level frameworks.
                    Instead of relying on an existing HTTP server library,
                    ForgeHTTP implements the core networking and request
                    processing pipeline itself.
                </p>

                <div className="tech-grid">
                    <div className="tech-item">
                        <span className="tech-name">C++20</span>
                        <span className="tech-description">
                            Core implementation language
                        </span>
                    </div>

                    <div className="tech-item">
                        <span className="tech-name">POSIX SOCKETS</span>
                        <span className="tech-description">
                            TCP network communication
                        </span>
                    </div>

                    <div className="tech-item">
                        <span className="tech-name">HTTP/1.1</span>
                        <span className="tech-description">
                            HTTP request and response handling
                        </span>
                    </div>

                    <div className="tech-item">
                        <span className="tech-name">THREAD POOL</span>
                        <span className="tech-description">
                            Concurrent request processing
                        </span>
                    </div>

                    <div className="tech-item">
                        <span className="tech-name">MIDDLEWARE</span>
                        <span className="tech-description">
                            Request processing pipeline
                        </span>
                    </div>

                    <div className="tech-item">
                        <span className="tech-name">ROUTING</span>
                        <span className="tech-description">
                            HTTP method and path matching
                        </span>
                    </div>
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
                    <div className="pipeline-step">
                        <span className="pipeline-number">01</span>
                        <strong>CLIENT</strong>
                        <span>HTTP request</span>
                    </div>

                    <div className="pipeline-arrow">↓</div>

                    <div className="pipeline-step">
                        <span className="pipeline-number">02</span>
                        <strong>TCP SOCKET</strong>
                        <span>Accept connection</span>
                    </div>

                    <div className="pipeline-arrow">↓</div>

                    <div className="pipeline-step">
                        <span className="pipeline-number">03</span>
                        <strong>HTTP PARSER</strong>
                        <span>Parse request</span>
                    </div>

                    <div className="pipeline-arrow">↓</div>

                    <div className="pipeline-step">
                        <span className="pipeline-number">04</span>
                        <strong>THREAD POOL</strong>
                        <span>Process concurrently</span>
                    </div>

                    <div className="pipeline-arrow">↓</div>

                    <div className="pipeline-step">
                        <span className="pipeline-number">05</span>
                        <strong>MIDDLEWARE</strong>
                        <span>Process request</span>
                    </div>

                    <div className="pipeline-arrow">↓</div>

                    <div className="pipeline-step">
                        <span className="pipeline-number">06</span>
                        <strong>ROUTER</strong>
                        <span>Match endpoint</span>
                    </div>

                    <div className="pipeline-arrow">↓</div>

                    <div className="pipeline-step">
                        <span className="pipeline-number">07</span>
                        <strong>HANDLER</strong>
                        <span>Generate result</span>
                    </div>

                    <div className="pipeline-arrow">↓</div>

                    <div className="pipeline-step">
                        <span className="pipeline-number">08</span>
                        <strong>HTTP RESPONSE</strong>
                        <span>Send to client</span>
                    </div>
                </div>
            </section>

                <section id="dashboard" className="intro-section">
                <div className="section-label">03 / DASHBOARD</div>

                <h2>Using the dashboard</h2>

                <p>
                    The dashboard provides a visual interface for exploring
                    the server and its runtime behaviour.
                </p>

                <div className="dashboard-list">
                    <div className="dashboard-item">
                        <span className="dashboard-command">&gt; INTRODUCTION</span>
                        <span>
                            Learn about ForgeHTTP and how the dashboard works.
                        </span>
                    </div>

                    <div className="dashboard-item">
                        <span className="dashboard-command">&gt; DASHBOARD</span>
                        <span>
                            View the overall state and activity of the server.
                        </span>
                    </div>

                    <div className="dashboard-item">
                        <span className="dashboard-command">&gt; REQUESTS</span>
                        <span>
                            Inspect HTTP request activity.
                        </span>
                    </div>

                    <div className="dashboard-item">
                        <span className="dashboard-command">&gt; ROUTES</span>
                        <span>
                            Explore registered HTTP routes.
                        </span>
                    </div>

                    <div className="dashboard-item">
                        <span className="dashboard-command">&gt; MIDDLEWARE</span>
                        <span>
                            Visualise the request processing pipeline.
                        </span>
                    </div>

                    <div className="dashboard-item">
                        <span className="dashboard-command">&gt; METRICS</span>
                        <span>
                            Monitor server performance and activity.
                        </span>
                    </div>

                    <div className="dashboard-item">
                        <span className="dashboard-command">&gt; HEALTH</span>
                        <span>
                            Check whether the server is responding.
                        </span>
                    </div>

                    <div className="dashboard-item">
                        <span className="dashboard-command">
                            &gt; CONFIGURATION
                        </span>
                        <span>
                            View the current server configuration.
                        </span>
                    </div>
                </div>
            </section>

            <section id="quick-start" className="intro-section intro-quick-start">
                <div className="section-label">04 / QUICK START</div>

                <h2>Get started</h2>

                <div className="quick-start">
                    <div>
                        <span>01</span>
                        Start ForgeHTTP.
                    </div>

                    <div>
                        <span>02</span>
                        Start the React dashboard.
                    </div>

                    <div>
                        <span>03</span>
                        Check the server status.
                    </div>

                    <div>
                        <span>04</span>
                        Open the Dashboard.
                    </div>

                    <div>
                        <span>05</span>
                        Explore Requests and Metrics.
                    </div>

                    <div>
                        <span>06</span>
                        Inspect Routes and Middleware.
                    </div>
                </div>
            </section>
        </div>
    );
}

export default Introduction;