import { useEffect, useState } from "react";

import StatCard from "../components/statCard";
import * as api from "../services/api";
import type { Metrics } from "../types/api";

import "../styles/dashboard.css";

function Dashboard() {
    const [metrics, setMetrics] = useState<Metrics | null>(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState<string | null>(null);

    useEffect(() => {
        async function loadMetrics() {
            try {
                const data = await api.getMetrics();
                setMetrics(data);
                setError(null);
            } catch {
                setError("Unable to retrieve server metrics.");
            } finally {
                setLoading(false);
            }
        }

        loadMetrics();
    }, []);

    return (
        <div className="dashboard-page">
            <div className="page-header">
                <div>
                    <span className="page-label">SERVER / OVERVIEW</span>
                    <h1>Dashboard</h1>
                    <p>
                        Real-time overview of the ForgeHTTP server.
                    </p>
                </div>
            </div>

            {loading && (
                <div className="dashboard-message">
                    LOADING METRICS...
                </div>
            )}

            {error && (
                <div className="dashboard-message dashboard-error">
                    <strong>FORGEHTTP UNAVAILABLE</strong>
                    <span>{error}</span>
                </div>
            )}

            {metrics && !loading && !error && (
                <>
                    <section className="stats-grid">
                        <StatCard
                            label="TOTAL REQUESTS"
                            value={metrics.requests.toLocaleString()}
                            description="Requests received"
                        />

                        <StatCard
                            label="ACTIVE CONNECTIONS"
                            value={metrics.active_connections}
                            description="Current connections"
                        />

                        <StatCard
                            label="AVG RESPONSE TIME"
                            value={`${metrics.average_response_time_ms} ms`}
                            description="Average response time"
                        />

                        <StatCard
                            label="ERRORS"
                            value={metrics.errors}
                            description="Recorded errors"
                        />

                        <StatCard
                            label="WORKERS"
                            value={metrics.workers}
                            description="Thread pool workers"
                        />
                    </section>

                    <section className="dashboard-section">
                        <div className="section-heading">
                            <span className="page-label">
                                SERVER STATUS
                            </span>

                            <h2>Runtime overview</h2>
                        </div>

                        <div className="runtime-panel">
                            <div className="runtime-row">
                                <span>HTTP SERVER</span>
                                <span className="runtime-value">
                                    ACTIVE
                                </span>
                            </div>

                            <div className="runtime-row">
                                <span>THREAD POOL</span>
                                <span className="runtime-value">
                                    {metrics.workers} WORKERS
                                </span>
                            </div>

                            <div className="runtime-row">
                                <span>REQUEST PROCESSING</span>
                                <span className="runtime-value">
                                    OPERATIONAL
                                </span>
                            </div>

                            <div className="runtime-row">
                                <span>METRICS</span>
                                <span className="runtime-value">
                                    AVAILABLE
                                </span>
                            </div>
                        </div>
                    </section>
                </>
            )}
        </div>
    );
}

export default Dashboard;