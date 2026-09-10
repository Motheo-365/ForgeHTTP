import { useEffect, useState } from "react";

import type { HealthResponse } from "../types/api";

import { getHealth } from "../services/api";
import { useMetrics } from "../context/metricsContext";


import "../styles/health.css";

function Health() {
    const { metrics, loading: metricsLoading } = useMetrics();

    const [health, setHealth] = useState<HealthResponse | null>(null);
    const [loading, setLoading] = useState(true);
    const [lastChecked, setLastChecked] = useState<Date | null>(null);

    async function checkHealth(): Promise<void> {
        setLoading(true);

        try {
            const data = await getHealth();

            setHealth(data);
            setLastChecked(new Date());
        } catch {
            setHealth(null);
            setLastChecked(new Date());
        } finally {
            setLoading(false);
        }
    }

    useEffect(() => {
        checkHealth();

        const interval = setInterval(checkHealth, 5000);

        return () => clearInterval(interval);
    }, []);

    const serverHealthy =
        health?.status === "ok";

    const formatTime = (date: Date | null): string => {
        if (!date) {
            return "--:--:--";
        }

        return date.toLocaleTimeString([], {
            hour: "2-digit",
            minute: "2-digit",
            second: "2-digit"
        });
    };

    return (
        <main className="health-page">
            <header className="health-page-header">
                <div>
                    <span className="page-label">
                        SYSTEM STATUS
                    </span>

                    <h1>Health</h1>

                    <p>
                        Monitor the availability and operational
                        status of the ForgeHTTP server.
                    </p>
                </div>

                <button
                    className="health-check-button"
                    onClick={checkHealth}
                >
                    CHECK NOW
                </button>
            </header>

            <section className="health-status-panel">
                <div className="health-status-indicator">
                    <span
                        className={
                            serverHealthy
                                ? "health-status-dot"
                                : "health-status-dot health-status-dot-error"
                        }
                    />
                </div>

                <div className="health-status-content">
                    <span className="health-status-label">
                        SERVER STATUS
                    </span>

                    <h2>
                        {loading
                            ? "CHECKING"
                            : serverHealthy
                                ? "HEALTHY"
                                : "UNAVAILABLE"}
                    </h2>

                    <p>
                        {loading
                            ? "Checking the ForgeHTTP health endpoint..."
                            : serverHealthy
                                ? "ForgeHTTP is responding normally."
                                : "The ForgeHTTP server could not be reached."}
                    </p>
                </div>

                <div className="health-status-meta">
                    <span>LAST CHECK</span>
                    <strong>
                        {formatTime(lastChecked)}
                    </strong>
                </div>
            </section>

            <section className="health-checks-section">
                <div className="health-section-header">
                    <div>
                        <span className="page-label">
                            DIAGNOSTICS
                        </span>

                        <h2>System checks</h2>
                    </div>
                </div>

                <div className="health-checks">
                    <div className="health-check">
                        <div>
                            <strong>HTTP Server</strong>
                            <span>
                                Core HTTP server availability
                            </span>
                        </div>

                        <span
                            className={
                                serverHealthy
                                    ? "health-check-status"
                                    : "health-check-status health-check-status-error"
                            }
                        >
                            {serverHealthy ? "HEALTHY" : "FAILED"}
                        </span>
                    </div>

                    <div className="health-check">
                        <div>
                            <strong>Health Endpoint</strong>
                            <span>
                                GET /health
                            </span>
                        </div>

                        <span
                            className={
                                serverHealthy
                                    ? "health-check-status"
                                    : "health-check-status health-check-status-error"
                            }
                        >
                            {serverHealthy ? "200 OK" : "FAILED"}
                        </span>
                    </div>

                    <div className="health-check">
                        <div>
                            <strong>Metrics</strong>
                            <span>
                                Runtime metrics availability
                            </span>
                        </div>

                        <span
                            className={
                                !metricsLoading && metrics
                                    ? "health-check-status"
                                    : "health-check-status health-check-status-error"
                            }
                        >
                            {!metricsLoading && metrics
                                ? "HEALTHY"
                                : "UNAVAILABLE"}
                        </span>
                    </div>

                    <div className="health-check">
                        <div>
                            <strong>Request Monitoring</strong>
                            <span>
                                Request history collection
                            </span>
                        </div>

                        <span className="health-check-status">
                            ACTIVE
                        </span>
                    </div>
                </div>
            </section>

            <section className="health-runtime-section">
                <div className="health-section-header">
                    <div>
                        <span className="page-label">
                            RUNTIME
                        </span>

                        <h2>Server runtime</h2>
                    </div>
                </div>

                <div className="health-runtime-grid">
                    <div className="health-runtime-item">
                        <span>WORKER THREADS</span>
                        <strong>
                            {metrics?.workers ?? "--"}
                        </strong>
                    </div>

                    <div className="health-runtime-item">
                        <span>ACTIVE CONNECTIONS</span>
                        <strong>
                            {metrics?.active_connections ?? "--"}
                        </strong>
                    </div>

                    <div className="health-runtime-item">
                        <span>TOTAL REQUESTS</span>
                        <strong>
                            {metrics?.requests ?? "--"}
                        </strong>
                    </div>

                    <div className="health-runtime-item">
                        <span>AVG RESPONSE TIME</span>
                        <strong>
                            {metrics
                                ? `${metrics.average_response_time_ms.toFixed(2)} ms`
                                : "--"}
                        </strong>
                    </div>
                </div>
            </section>

            <section className="health-response-section">
                <div className="health-section-header">
                    <div>
                        <span className="page-label">
                            ENDPOINT RESPONSE
                        </span>

                        <h2>Health response</h2>
                    </div>
                </div>

                <pre className="health-response">
                    {health
                        ? JSON.stringify(health, null, 4)
                        : "NO RESPONSE"}
                </pre>
            </section>
        </main>
    );
}

export default Health;