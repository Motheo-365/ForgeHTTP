import { useMetrics } from "../context/metricsContext";
import "../styles/metrics.css";

function Metrics() {
    const {
        metrics,
        metricsHistory,
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

    const responseTimes = metricsHistory.map(
        (entry) => entry.average_response_time_ms
    );

    const requests = metricsHistory.map(
        (entry) => entry.requests
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

    const createPoints = (values, maxValue) => {
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

    const formatTime = (index) => {
        if (!metricsHistory[index]) {
            return "";
        }

        return metricsHistory[index].time;
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
                <div className="metric-stat">
                    <span className="metric-stat-label">
                        REQUESTS
                    </span>

                    <strong>
                        {metrics?.requests ?? 0}
                    </strong>
                </div>

                <div className="metric-stat">
                    <span className="metric-stat-label">
                        ACTIVE CONNECTIONS
                    </span>

                    <strong>
                        {metrics?.active_connections ?? 0}
                    </strong>
                </div>

                <div className="metric-stat metric-stat-response">
                    <span className="metric-stat-label">
                        AVG RESPONSE TIME
                    </span>

                    <strong>
                        {(
                            metrics?.average_response_time_ms ?? 0
                        ).toFixed(2)}

                        <small> ms</small>
                    </strong>
                </div>

                <div className="metric-stat metric-stat-error">
                    <span className="metric-stat-label">
                        ERRORS
                    </span>

                    <strong>
                        {metrics?.errors ?? 0}
                    </strong>
                </div>

                <div className="metric-stat">
                    <span className="metric-stat-label">
                        WORKERS
                    </span>

                    <strong>
                        {metrics?.workers ?? 0}
                    </strong>
                </div>
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
                            {metricsHistory.length} samples
                        </span>
                    </div>

                    <div className="metrics-chart">
                        {metricsHistory.length < 2 ? (

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
                                                    (metricsHistory.length - 1) /
                                                    2
                                                )
                                            )}
                                        </span>

                                        <span>
                                            {formatTime(
                                                metricsHistory.length - 1
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
                            {metricsHistory.length} samples
                        </span>
                    </div>

                    <div className="metrics-chart">
                        {metricsHistory.length < 2 ? (
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
                                                    (metricsHistory.length - 1) /
                                                    2
                                                )
                                            )}
                                        </span>

                                        <span>
                                            {formatTime(
                                                metricsHistory.length - 1
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
                    <div className="runtime-item">
                        <span>
                            REQUEST COUNT
                        </span>

                        <strong>
                            {metrics?.requests ?? 0}
                        </strong>
                    </div>

                    <div className="runtime-item">
                        <span>
                            ACTIVE CONNECTIONS
                        </span>

                        <strong>
                            {metrics?.active_connections ?? 0}
                        </strong>
                    </div>

                    <div className="runtime-item">
                        <span>
                            WORKER THREADS
                        </span>

                        <strong>
                            {metrics?.workers ?? 0}
                        </strong>
                    </div>


                    <div className="runtime-item">
                        <span>
                            ERROR COUNT
                        </span>

                        <strong>
                            {metrics?.errors ?? 0}
                        </strong>
                    </div>                </div>
            </section>
        </main>
    );
}

export default Metrics;