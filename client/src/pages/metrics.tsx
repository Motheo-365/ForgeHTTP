import { useMetrics } from "../context/metricsContext";
import StatCard from "../components/statCard";
import "../styles/metrics.css";

function Metrics() {
    const {
        metrics,
        requestHistory,
        loading,
        error
    } = useMetrics();

    if (loading && !metrics) {
        return (
            <main className="metrics-page">
                <div className="metrics-empty">
                    LOADING METRICS...
                </div>
            </main>
        );
    }

    if (error && !metrics) {
        return (
            <main className="metrics-page">
                <div className="metrics-empty metrics-error">
                    {error}
                </div>
            </main>
        );
    }

    const chronologicalRequests = [...requestHistory].reverse();

    const responseTimes = chronologicalRequests.map(
        (entry) => entry.duration_ms
    );

    const requests = chronologicalRequests.map(
        (_, index) => index + 1
    );

    const maxResponseTime = Math.max(
        ...responseTimes,
        1
    );

    const maxRequests = Math.max(
        ...requests,
        1
    );

    const chartWidth = 100;
    const chartHeight = 100;

    const createPoints = (
        values: number[],
        maxValue: number
    ): string[] => {
        return values.map((value, index) => {
            const x =
                values.length === 1
                    ? chartWidth / 2
                    : (index / (values.length - 1)) * chartWidth;

            const y =
                chartHeight -
                (value / maxValue) * 80 -
                10;

            return `${x},${y}`;
        });
    };

    const responsePoints = createPoints(
        responseTimes,
        maxResponseTime
    );

    const requestPoints = createPoints(
        requests,
        maxRequests
    );

    const formatTime = (index: number): string => {
        if (!chronologicalRequests[index]) {
            return "";
        }

        return chronologicalRequests[index].time;
    };

    return (
        <main className="metrics-page">
            <header className="metrics-page-header">
                <div>
                    <span className="page-label">
                        OBSERVABILITY
                    </span>

                    <h1>Metrics</h1>

                    <p>
                        Live server performance and runtime statistics.
                    </p>
                </div>

                <div className="metrics-live-status">
                    <span className="metrics-live-dot"></span>
                    LIVE
                </div>
            </header>

            <section className="metrics-overview">
                <StatCard label="REQUESTS" value={metrics?.requests ?? 0} />
                <StatCard label="ACTIVE CONNECTIONS" value={metrics?.active_connections ?? 0} />
                <StatCard
                    label="AVG RESPONSE TIME"
                    value={`${(metrics?.average_response_time_ms ?? 0).toFixed(2)} ms`}
                />
                <StatCard label="ERRORS" value={metrics?.errors ?? 0} />
                <StatCard label="WORKERS" value={metrics?.workers ?? 0} />
            </section>

            <section className="metrics-charts-grid">
                {/* RESPONSE TIME */}
                <section className="metrics-chart-section">
                    <div className="metrics-section-header">
                        <div>
                            <span className="page-label">
                                PERFORMANCE
                            </span>

                            <h2>
                                Response time
                            </h2>
                        </div>

                        <span className="metrics-chart-meta">
                            {requestHistory.length} requests
                        </span>
                    </div>

                    <div className="metrics-chart">
                        {requestHistory.length < 2 ? (

                            <div className="metrics-chart-empty">
                                COLLECTING PERFORMANCE DATA...
                            </div>

                        ) : (
                            <div className="chart-container">
                                <div className="chart-y-axis">
                                    <span>
                                        {maxResponseTime.toFixed(1)}
                                    </span>

                                    <span>
                                        {(maxResponseTime * 0.75).toFixed(1)}
                                    </span>

                                    <span>
                                        {(maxResponseTime * 0.5).toFixed(1)}
                                    </span>

                                    <span>
                                        {(maxResponseTime * 0.25).toFixed(1)}
                                    </span>

                                    <span>
                                        0
                                    </span>
                                </div>

                                <div className="chart-main">
                                    <div className="chart-grid-lines">

                                        <span></span>
                                        <span></span>
                                        <span></span>
                                        <span></span>
                                        <span></span>

                                    </div>

                                    <svg
                                        viewBox="0 0 100 100"
                                        preserveAspectRatio="none"
                                        className="metrics-chart-svg"
                                    >
                                        <polyline
                                            points={responsePoints.join(" ")}
                                            fill="none"
                                            stroke="currentColor"
                                            strokeWidth="1.5"
                                            vectorEffect="non-scaling-stroke"
                                        />

                                    </svg>

                                    <div className="chart-x-axis">
                                        <span>
                                            {formatTime(0)}
                                        </span>

                                        <span>
                                            {formatTime(
                                                Math.floor(
                                                    (requestHistory.length - 1) /
                                                    2
                                                )
                                            )}
                                        </span>

                                        <span>
                                            {formatTime(
                                                requestHistory.length - 1
                                            )}
                                        </span>

                                    </div>

                                </div>

                                <div className="chart-axis-label chart-y-label">
                                    RESPONSE TIME (MS)
                                </div>

                                <div className="chart-axis-label chart-x-label">
                                    TIME
                                </div>
                            </div>
                        )}
                    </div>
                </section>

                {/* REQUESTS */}
                <section className="metrics-chart-section">
                    <div className="metrics-section-header">
                        <div>
                            <span className="page-label">
                                TRAFFIC
                            </span>

                            <h2>
                                Requests over time
                            </h2>
                        </div>

                        <span className="metrics-chart-meta">
                            {requestHistory.length} requests
                        </span>
                    </div>

                    <div className="metrics-chart">
                        {requestHistory.length < 2 ? (
                            <div className="metrics-chart-empty">
                                COLLECTING REQUEST DATA...
                            </div>
                        ) : (
                            <div className="chart-container">
                                <div className="chart-y-axis">
                                    <span>
                                        {maxRequests}
                                    </span>

                                    <span>
                                        {Math.round(
                                            maxRequests * 0.75
                                        )}
                                    </span>

                                    <span>
                                        {Math.round(
                                            maxRequests * 0.5
                                        )}
                                    </span>

                                    <span>
                                        {Math.round(
                                            maxRequests * 0.25
                                        )}
                                    </span>

                                    <span>
                                        0
                                    </span>
                                </div>

                                <div className="chart-main">
                                    <div className="chart-grid-lines">
                                        <span></span>
                                        <span></span>
                                        <span></span>
                                        <span></span>
                                        <span></span>
                                    </div>

                                    <svg
                                        viewBox="0 0 100 100"
                                        preserveAspectRatio="none"
                                        className="metrics-chart-svg"
                                    >
                                        <polyline
                                            points={requestPoints.join(" ")}
                                            fill="none"
                                            stroke="currentColor"
                                            strokeWidth="1.5"
                                            vectorEffect="non-scaling-stroke"
                                        />
                                    </svg>

                                    <div className="chart-x-axis">
                                        <span>
                                            {formatTime(0)}
                                        </span>

                                        <span>
                                            {formatTime(
                                                Math.floor(
                                                    (requestHistory.length - 1) /
                                                    2
                                                )
                                            )}
                                        </span>

                                        <span>
                                            {formatTime(
                                                requestHistory.length - 1
                                            )}
                                        </span>
                                    </div>
                                </div>

                                <div className="chart-axis-label chart-y-label">
                                    REQUESTS
                                </div>

                                <div className="chart-axis-label chart-x-label">
                                    TIME
                                </div>
                            </div>
                        )}
                    </div>
                </section>
            </section>

            <section className="metrics-runtime">
                <div className="metrics-section-header">
                    <div>
                        <span className="page-label">
                            RUNTIME
                        </span>

                        <h2>
                            Server state
                        </h2>
                    </div>
                </div>

                <div className="runtime-grid">
                    <StatCard label="REQUEST COUNT" value={metrics?.requests ?? 0} />
                    <StatCard label="ACTIVE CONNECTIONS" value={metrics?.active_connections ?? 0} />
                    <StatCard label="WORKER THREADS" value={metrics?.workers ?? 0} />
                    <StatCard label="ERROR COUNT" value={metrics?.errors ?? 0} />
                </div>
            </section>
        </main>
    );
}

export default Metrics;