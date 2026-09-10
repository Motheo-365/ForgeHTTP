import { useMetrics } from "../context/metricsContext";
import StatCard from "../components/statCard";

import "../styles/requests.css";

function Requests() {
    const { requestHistory, metrics } = useMetrics();

    const totalRequests = requestHistory.length;

    const successfulRequests = requestHistory.filter(
        (request) =>
            request.status >= 200 &&
            request.status < 300
    ).length;

    const clientErrors = requestHistory.filter(
        (request) =>
            request.status >= 400 &&
            request.status < 500
    ).length;

    const serverErrors = requestHistory.filter(
        (request) => request.status >= 500
    ).length;

    return (
        <main className="requests-page">
            <header className="requests-page-header">
                <div>
                    <span className="page-label">
                        OBSERVABILITY
                    </span>

                    <h1>Request history</h1>

                    <p>
                        Monitor incoming HTTP requests and
                        response performance.
                    </p>
                </div>
            </header>

            <section className="requests-overview">
                <StatCard
                    label="TOTAL REQUESTS"
                    value={(metrics?.requests ?? totalRequests).toLocaleString()}
                    description="Requests received"
                />

                <StatCard label="SUCCESSFUL" value={successfulRequests} />
                <StatCard label="CLIENT ERRORS" value={clientErrors} />
                <StatCard label="SERVER ERRORS" value={serverErrors} />
            </section>

            <section className="requests-table-section">
                <div className="requests-section-header">
                    <div>
                        <span className="page-label">
                            REQUEST LOG
                        </span>

                        <h2>All requests</h2>
                    </div>
                </div>

                {requestHistory.length === 0 ? (
                    <div className="requests-empty">
                        NO REQUEST DATA AVAILABLE
                    </div>
                ) : (
                    <div className="requests-table-wrapper">
                        <table>
                            <thead>
                                <tr>
                                    <th>TIME</th>
                                    <th>METHOD</th>
                                    <th>PATH</th>
                                    <th>STATUS</th>
                                    <th>DURATION</th>
                                </tr>
                            </thead>

                            <tbody>
                                {requestHistory.map(
                                    (request, index) => (
                                        <tr key={index}>
                                            <td>
                                                {request.time}
                                            </td>

                                            <td>
                                                {request.method}
                                            </td>

                                            <td>
                                                {request.path}
                                            </td>

                                            <td
                                                className={
                                                    request.status >= 400
                                                        ? "request-error"
                                                        : ""
                                                }
                                            >
                                                {request.status}
                                            </td>

                                            <td>
                                                {request.duration_ms.toFixed(2)} ms
                                            </td>
                                        </tr>
                                    )
                                )}
                            </tbody>
                        </table>
                    </div>
                )}
            </section>
        </main>
    );
}

export default Requests;