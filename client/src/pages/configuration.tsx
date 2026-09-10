import { useMetrics } from "../context/metricsContext";

import "../styles/configuration.css";

function Configuration() {
    const { metrics } = useMetrics();

    const serverConfiguration = [
        {
            label: "HTTP VERSION",
            value: "HTTP/1.1"
        },
        {
            label: "PORT",
            value: "8086"
        },
        {
            label: "WORKER THREADS",
            value: metrics?.workers ?? "8"
        },
        {
            label: "RATE LIMIT",
            value: "100 requests"
        }
    ];

    const middleware = [
        {
            name: "CORS",
            description: "Cross-origin request handling"
        },
        {
            name: "LOGGER",
            description: "Request activity logging"
        },
        {
            name: "RATE LIMITER",
            description: "Request rate protection"
        },
        {
            name: "AUTH",
            description: "Protected API authorization"
        }
    ];

    const endpoints = [
        {
            name: "HEALTH",
            path: "/health"
        },
        {
            name: "METRICS",
            path: "/metrics"
        },
        {
            name: "REQUEST HISTORY",
            path: "/api/requests"
        },
        {
            name: "USERS",
            path: "/api/users"
        }
    ];

    return (
        <main className="configuration-page">
            <header className="configuration-page-header">
                <div>
                    <span className="page-label">
                        SERVER SETTINGS
                    </span>

                    <h1>Configuration</h1>

                    <p>
                        Current ForgeHTTP server configuration
                        and registered components.
                    </p>
                </div>

                <span className="configuration-readonly">
                    READ ONLY
                </span>
            </header>

            <section className="configuration-section">
                <div className="configuration-section-header">
                    <div>
                        <span className="page-label">
                            SERVER
                        </span>

                        <h2>Server configuration</h2>
                    </div>
                </div>

                <div className="configuration-grid">
                    {serverConfiguration.map((item) => (
                        <div
                            className="configuration-item"
                            key={item.label}
                        >
                            <span>
                                {item.label}
                            </span>

                            <strong>
                                {item.value}
                            </strong>
                        </div>
                    ))}
                </div>
            </section>

            <section className="configuration-section">
                <div className="configuration-section-header">
                    <div>
                        <span className="page-label">
                            REQUEST PIPELINE
                        </span>

                        <h2>Middleware</h2>
                    </div>
                </div>

                <div className="configuration-list">
                    {middleware.map((item, index) => (
                        <div
                            className="configuration-row"
                            key={item.name}
                        >
                            <span className="configuration-order">
                                {String(index + 1).padStart(2, "0")}
                            </span>

                            <div className="configuration-row-content">
                                <strong>
                                    {item.name}
                                </strong>

                                <span>
                                    {item.description}
                                </span>
                            </div>

                            <span className="configuration-active">
                                ACTIVE
                            </span>
                        </div>
                    ))}
                </div>
            </section>

            <section className="configuration-section">
                <div className="configuration-section-header">
                    <div>
                        <span className="page-label">
                            ROUTING
                        </span>

                        <h2>Registered endpoints</h2>
                    </div>
                </div>

                <div className="configuration-list">
                    {endpoints.map((endpoint) => (
                        <div
                            className="configuration-row"
                            key={endpoint.path}
                        >
                            <div className="configuration-row-content">
                                <strong>
                                    {endpoint.name}
                                </strong>
                            </div>

                            <code>
                                {endpoint.path}
                            </code>
                        </div>
                    ))}
                </div>
            </section>
        </main>
    );
}

export default Configuration;