import StatCard from "../components/statCard";
import "../styles/routes.css";

function RoutesPage() {
    const routes = [
        {
            method: "GET",
            path: "/health",
            handler: "HealthController",
            access: "PUBLIC"
        },
        {
            method: "GET",
            path: "/api/users",
            handler: "UserController",
            access: "PROTECTED"
        },
        {
            method: "POST",
            path: "/api/users",
            handler: "UserController",
            access: "PROTECTED"
        },
        {
            method: "GET",
            path: "/api/requests",
            handler: "RequestHistory",
            access: "PROTECTED"
        },
        {
            method: "GET",
            path: "/metrics",
            handler: "MetricsConnector",
            access: "PUBLIC"
        }
    ];

    const publicRoutes = routes.filter(
        (route) => route.access === "PUBLIC"
    ).length;

    const protectedRoutes = routes.filter(
        (route) => route.access === "PROTECTED"
    ).length;

    return (
        <main className="routes-page">
            <header className="routes-page-header">
                <div>
                    <span className="page-label">
                        ROUTING
                    </span>

                    <h1>Routes</h1>

                    <p>
                        Registered HTTP endpoints exposed by ForgeHTTP.
                    </p>
                </div>
            </header>

            <section className="routes-overview">
                <StatCard label="TOTAL ROUTES" value={routes.length} />
                <StatCard label="PUBLIC" value={publicRoutes} />
                <StatCard label="PROTECTED" value={protectedRoutes} />
            </section>

            <section className="routes-table-section">
                <div className="routes-section-header">
                    <div>
                        <span className="page-label">
                            ROUTE REGISTRY
                        </span>

                        <h2>Registered endpoints</h2>
                    </div>
                </div>

                <div className="routes-table-wrapper">
                    <table>
                        <thead>
                            <tr>
                                <th>METHOD</th>
                                <th>PATH</th>
                                <th>HANDLER</th>
                                <th>ACCESS</th>
                            </tr>
                        </thead>

                        <tbody>
                            {routes.map((route, index) => (
                                <tr key={index}>
                                    <td>
                                        <span
                                            className={`route-method route-method-${route.method.toLowerCase()}`}
                                        >
                                            {route.method}
                                        </span>
                                    </td>

                                    <td className="route-path">
                                        {route.path}
                                    </td>

                                    <td>
                                        {route.handler}
                                    </td>

                                    <td>
                                        <span
                                            className={
                                                route.access === "PUBLIC"
                                                    ? "route-access-public"
                                                    : "route-access-protected"
                                            }
                                        >
                                            {route.access}
                                        </span>
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

export default RoutesPage;