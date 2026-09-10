import { useState } from "react";

import { useMetrics } from "../context/metricsContext";

import "../styles/requestTable.css";

function RequestTable() {
    const { metricsHistory } = useMetrics();

    const [sampleCount, setSampleCount] = useState(10);

    const recentRequests = [...metricsHistory]
        .slice(-sampleCount)
        .reverse();

    return (
        <section className="request-table">
            <div className="request-table-header">
                <div>
                    <span className="page-label">
                        RECENT ACTIVITY
                    </span>

                    <h2>Request history</h2>
                </div>

                <select
                    value={sampleCount}
                    onChange={(event) =>
                        setSampleCount(Number(event.target.value))
                    }
                >
                    <option value={10}>Last 10 samples</option>
                    <option value={25}>Last 25 samples</option>
                    <option value={50}>Last 50 samples</option>
                    <option value={100}>Last 100 samples</option>
                </select>
            </div>

            {recentRequests.length === 0 ? (
                <div className="request-table-empty">
                    NO REQUEST DATA AVAILABLE
                </div>
            ) : (
                <div className="request-table-wrapper">
                    <table>
                        <thead>
                            <tr>
                                <th>SAMPLE</th>
                                <th>REQUESTS</th>
                                <th>ACTIVE</th>
                                <th>AVG RESPONSE</th>
                                <th>ERRORS</th>
                            </tr>
                        </thead>

                        <tbody>
                            {recentRequests.map((metrics, index) => (
                                <tr key={index}>
                                    <td>
                                        {metricsHistory.length - index}
                                    </td>

                                    <td>
                                        {metrics.requests.toLocaleString()}
                                    </td>

                                    <td>
                                        {metrics.active_connections}
                                    </td>

                                    <td>
                                        {metrics.average_response_time_ms.toFixed(2)} ms
                                    </td>

                                    <td
                                        className={
                                            metrics.errors > 0
                                                ? "request-error"
                                                : ""
                                        }
                                    >
                                        {metrics.errors}
                                    </td>
                                </tr>
                            ))}
                        </tbody>
                    </table>
                </div>
            )}
        </section>
    );
}

export default RequestTable;