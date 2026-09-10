import StatCard from "../components/statCard";
import "../styles/middleware.css";

function Middleware() {
    const middleware = [
        {
            name: "CORS",
            purpose: "Controls cross-origin requests.",
            status: "ACTIVE"
        },
        {
            name: "Logger",
            purpose: "Records incoming request activity.",
            status: "ACTIVE"
        },
        {
            name: "Rate Limiter",
            purpose: "Limits excessive requests from clients.",
            status: "ACTIVE"
        },
        {
            name: "Auth",
            purpose: "Validates protected API requests.",
            status: "ACTIVE"
        }
    ];

    return (
        <main className="middleware-page">
            <header className="middleware-page-header">
                <div>
                    <span className="page-label">
                        REQUEST PIPELINE
                    </span>

                    <h1>Middleware</h1>

                    <p>
                        Request processing layers executed before
                        reaching the router.
                    </p>
                </div>
            </header>

            <section className="middleware-overview">
                <StatCard label="TOTAL MIDDLEWARE" value={middleware.length} />
                <StatCard
                    label="ACTIVE"
                    value={middleware.filter((item) => item.status === "ACTIVE").length}
                />
            </section>

            <section className="middleware-pipeline">
                <div className="middleware-section-header">
                    <div>
                        <span className="page-label">
                            EXECUTION ORDER
                        </span>

                        <h2>Request pipeline</h2>
                    </div>
                </div>

                <div className="middleware-flow">
                    {middleware.map((item, index) => (
                        <div
                            className="middleware-flow-item"
                            key={item.name}
                        >
                            <div className="middleware-flow-number">
                                0{index + 1}
                            </div>

                            <div className="middleware-flow-name">
                                {item.name}
                            </div>

                            {index < middleware.length - 1 && (
                                <span className="middleware-flow-arrow">
                                    →
                                </span>
                            )}
                        </div>
                    ))}
                    <span className="middleware-flow-arrow">
                        →
                    </span>
                    <div className="middleware-flow-item middleware-flow-router">
                        <div className="middleware-flow-number">
                            05
                        </div>

                        <div className="middleware-flow-name">
                            Router
                        </div>
                    </div>
                </div>
            </section>

            <section className="middleware-table-section">
                <div className="middleware-section-header">
                    <div>
                        <span className="page-label">
                            REGISTERED LAYERS
                        </span>

                        <h2>Middleware stack</h2>
                    </div>
                </div>

                <div className="middleware-table-wrapper">
                    <table>
                        <thead>
                            <tr>
                                <th>ORDER</th>
                                <th>NAME</th>
                                <th>PURPOSE</th>
                                <th>STATUS</th>
                            </tr>
                        </thead>

                        <tbody>
                            {middleware.map((item, index) => (
                                <tr key={item.name}>
                                    <td>
                                        {String(index + 1).padStart(2, "0")}
                                    </td>

                                    <td className="middleware-name">
                                        {item.name}
                                    </td>

                                    <td>
                                        {item.purpose}
                                    </td>

                                    <td className="middleware-status">
                                        {item.status}
                                    </td>
                                </tr>
                            ))}
                        </tbody>
                    </table>
                </div>
            </section>
        </main>
    );
}

export default Middleware;