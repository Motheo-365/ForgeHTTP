import { useState } from "react";
import { useMetrics } from "../context/metricsContext";

import "../styles/requestTable.css";

function RequestTable() {
    const { requestHistory } = useMetrics();
    
    const [sampleCount, setSampleCount] = useState(5);
    const recentRequests = requestHistory.slice(0, sampleCount);

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
                    <option value={5}>Last 5 requests</option>
                    <option value={10}>Last 10 requests</option>
                    <option value={25}>Last 25 requests</option>
                    <option value={50}>Last 50 requests</option>
                    <option value={100}>Last 100 requests</option>
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
                                <th>TIME</th>
                                <th>METHOD</th>
                                <th>PATH</th>
                                <th>STATUS</th>
                                <th>DURATION</th>
                            </tr>
                        </thead>

                        <tbody>
                            {recentRequests.map((request, index) => (
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
                            ))}
                        </tbody>
                    </table>
                </div>
            )}
        </section>
    );
}

export default RequestTable;