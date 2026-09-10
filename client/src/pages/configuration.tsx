import { useEffect, useState } from "react";

import type { ServerConfiguration } from "../types/api";

import {
    getConfiguration,
    updateConfiguration
} from "../services/api";

import "../styles/configuration.css";

function Configuration() {
    const [draft, setDraft] = useState<ServerConfiguration | null>(null);
    const [loading, setLoading] = useState(true);
    const [saving, setSaving] = useState(false);
    const [message, setMessage] = useState("");
    const [error, setError] = useState("");

    useEffect(() => {
        async function loadConfiguration(): Promise<void> {
            try {
                const data = await getConfiguration();
                setDraft(data);
            } catch (loadError) {
                setError(
                    loadError instanceof Error
                        ? loadError.message
                        : "Unable to load configuration."
                );
            } finally {
                setLoading(false);
            }
        }

        loadConfiguration();
    }, []);

    function updateDraft(
        field: keyof ServerConfiguration,
        value: number | boolean
    ): void {
        setDraft((current) => current
            ? { ...current, [field]: value }
            : current
        );
        setMessage("");
        setError("");
    }

    async function saveConfiguration(): Promise<void> {
        if (!draft) {
            return;
        }

        setSaving(true);
        setMessage("");
        setError("");

        try {
            const saved = await updateConfiguration(draft);
            setDraft(saved);
            setMessage("Configuration saved.");
        } catch (saveError) {
            setError(
                saveError instanceof Error
                    ? saveError.message
                    : "Unable to save configuration."
            );
        } finally {
            setSaving(false);
        }
    }

    const serverConfiguration = draft ? [
        {
            label: "HTTP VERSION",
            value: "HTTP/1.1"
        },
        {
            label: "PORT",
            field: "port" as const,
            value: draft.port,
            type: "number"
        },
        {
            label: "WORKER THREADS",
            field: "workerThreads" as const,
            value: draft.workerThreads,
            type: "number"
        },
        {
            label: "RATE LIMIT",
            field: "rateLimit" as const,
            value: draft.rateLimit,
            type: "number"
        }
    ] : [];

    const middleware = draft ? [
        {
            name: "CORS",
            description: "Cross-origin request handling",
            field: "corsEnabled" as const,
            active: draft.corsEnabled
        },
        {
            name: "LOGGER",
            description: "Request activity logging",
            field: "loggingEnabled" as const,
            active: draft.loggingEnabled
        },
        {
            name: "RATE LIMITER",
            description: "Request rate protection",
            active: true
        },
        {
            name: "AUTH",
            description: "Protected API authorization",
            field: "authenticationEnabled" as const,
            active: draft.authenticationEnabled
        }
    ] : [];

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
                    <span className="page-label">SERVER SETTINGS</span>
                    <h1>Configuration</h1>
                    <p>
                        Current ForgeHTTP server configuration
                        and registered components.
                    </p>
                </div>

                <button
                    className="configuration-save-button"
                    disabled={!draft || saving || loading}
                    onClick={saveConfiguration}
                >
                    {saving ? "SAVING..." : "SAVE CONFIGURATION"}
                </button>
            </header>

            {(message || error) && (
                <p className={error
                    ? "configuration-feedback configuration-feedback-error"
                    : "configuration-feedback"}
                >
                    {error || message}
                </p>
            )}

            <section className="configuration-section">
                <div className="configuration-section-header">
                    <div>
                        <span className="page-label">SERVER</span>
                        <h2>Server configuration</h2>
                    </div>

                    <a href="#instructions"><i>READ THIS BEFORE YOU START</i></a>
                </div>

                <div className="configuration-grid">
                    {loading && (
                        <div className="configuration-loading">
                            Loading configuration...
                        </div>
                    )}

                    {!loading && serverConfiguration.map((item) => (
                        <div className="configuration-item" key={item.label}>
                            <span>{item.label}</span>

                            {item.type === "number" ? (
                                <input
                                    className="configuration-input"
                                    type="number"
                                    min={1}
                                    max={
                                        item.field === "port"
                                            ? 65535
                                            : item.field === "workerThreads"
                                                ? 64
                                                : 10000
                                    }
                                    value={item.value}
                                    onChange={(event) => updateDraft(
                                        item.field,
                                        Number(event.target.value)
                                    )}
                                />
                            ) : (
                                <strong>{item.value}</strong>
                            )}
                        </div>
                    ))}
                </div>
            </section>

            <section className="configuration-section configuration-info-section">
                <div className="configuration-info-note">
                    <strong>FIRST TIME SETUP</strong>
                    <span>
                        If ForgeHTTP is not running, start with the
                        <code>SERVER GUIDE</code>
                        &#9; on the Introduction page. It explains how to open the PC terminal,
                        clone the repository, build the C++ backend, start ForgeHTTP, and
                        then start the React dashboard. The dashboard cannot change
                        configuration unless the backend is running and accessible.
                    </span>
                </div>

                <div className="configuration-info-note">
                    <strong>RESTARTING FORGEHTTP</strong>
                    <span>
                        When a setting marked
                        <code>RESTART REQUIRED</code>
                        &#9; is changed, save the configuration first. Then go to the PC terminal
                        where ForgeHTTP is running, press
                        <code>Ctrl+C</code>
                        &#9; to stop the server, and start it again using the commands shown on
                        the Introduction page. The new configuration will be loaded when
                        ForgeHTTP starts.
                    </span>
                </div>

                <div className="configuration-info-note">
                    <strong>LOCAL SERVER</strong>
                    <span>
                        The configuration controls your local ForgeHTTP backend. Changing
                        the port does not restart or modify the deployed server on Render.
                        For local development, the React dashboard must also use the same
                        port as the local ForgeHTTP backend.
                    </span>
                </div>
            </section>

            <section id="instructions" className="configuration-section configuration-info-section">
                <div className="configuration-section-header">
                    <div>
                        <span className="page-label">HOW CONFIGURATION WORKS</span>
                        <h2>Configuration behaviour</h2>
                    </div>
                </div>

                <div className="configuration-info-grid">
                    <div className="configuration-info-item">
                        <span className="configuration-info-status">SETUP REQUIRED</span>
                        <strong>BEFORE CHANGING CONFIGURATION</strong>
                        <p>
                            ForgeHTTP must be running locally before configuration can be
                            changed. If you have not already done so, open your PC terminal,
                            clone the ForgeHTTP repository as shown on the Introduction page,
                            build the C++ backend, and start the ForgeHTTP server. The React
                            dashboard communicates with this local backend.
                        </p>
                    </div>

                    <div className="configuration-info-item">
                        <span className="configuration-info-status">PC TERMINAL</span>
                        <strong>APPLYING CHANGES</strong>
                        <p>
                            Configuration values are changed using this dashboard, but the
                            ForgeHTTP server itself runs as a C++ process in your PC terminal.
                            Some changes can be applied immediately while others require you
                            to stop and restart the ForgeHTTP process from the PC terminal.
                        </p>
                    </div>

                    <div className="configuration-info-item">
                        <span className="configuration-info-status">RESTART REQUIRED</span>
                        <strong>PORT</strong>
                        <p>
                            Controls the TCP port used by ForgeHTTP. Change the value here,
                            save the configuration, then stop ForgeHTTP in the PC terminal
                            with Ctrl+C and start it again. The server will then listen on
                            the new port. For example, changing 8086 to 9000 makes the local
                            server available at http://localhost:9000 after the restart.
                        </p>
                    </div>

                    <div className="configuration-info-item">
                        <span className="configuration-info-status">RESTART REQUIRED</span>
                        <strong>WORKER THREADS</strong>
                        <p>
                            Controls how many worker threads ForgeHTTP creates to process
                            requests concurrently. Change the value here and save it, then
                            stop and restart the ForgeHTTP server from the PC terminal.
                            The new thread count is created when the server starts again.
                        </p>
                    </div>

                    <div className="configuration-info-item">
                        <span className="configuration-info-status configuration-info-live">LIVE</span>
                        <strong>RATE LIMIT</strong>
                        <p>
                            Controls how many requests the rate limiter allows. Change the
                            value here and save the configuration. The new rate limit is
                            applied to the running ForgeHTTP server without restarting it.
                        </p>
                    </div>

                    <div className="configuration-info-item">
                        <span className="configuration-info-status">CONFIGURATION</span>
                        <strong>HTTP VERSION</strong>
                        <p>
                            Displays the HTTP protocol implemented by ForgeHTTP. The current
                            server uses HTTP/1.1. This is an informational value and cannot
                            be changed from the dashboard.
                        </p>
                    </div>

                    <div className="configuration-info-item">
                        <span className="configuration-info-status">CONFIGURATION</span>
                        <strong>CORS</strong>
                        <p>
                            Stores the CORS setting used by ForgeHTTP. CORS controls whether
                            requests from a different origin, such as the React dashboard,
                            are allowed to communicate with the server. The setting is
                            currently stored as configuration and is not yet a live
                            middleware switch.
                        </p>
                    </div>

                    <div className="configuration-info-item">
                        <span className="configuration-info-status">CONFIGURATION</span>
                        <strong>LOGGER</strong>
                        <p>
                            Stores the logging setting for ForgeHTTP. The logger records
                            request activity while the server is running. The configuration
                            value can be changed here, but the dashboard does not currently
                            enable or disable the logging middleware at runtime.
                        </p>
                    </div>

                    <div className="configuration-info-item">
                        <span className="configuration-info-status">CONFIGURATION</span>
                        <strong>AUTH</strong>
                        <p>
                            Stores the authorization setting for protected API endpoints.
                            ForgeHTTP currently protects its API routes using a bearer
                            token. The configuration value is displayed here, but changing
                            it does not currently enable or disable authorization at runtime.
                        </p>
                    </div>
                </div>
            </section>

            <section className="configuration-section">
                <div className="configuration-section-header">
                    <div>
                        <span className="page-label">REQUEST PIPELINE</span>
                        <h2>Middleware</h2>
                    </div>
                </div>

                <div className="configuration-list">
                    {middleware.map((item, index) => (
                        <div className="configuration-row" key={item.name}>
                            <span className="configuration-order">
                                {String(index + 1).padStart(2, "0")}
                            </span>

                            <div className="configuration-row-content">
                                <strong>{item.name}</strong>
                                <span>{item.description}</span>
                            </div>

                            {item.field ? (
                                <label className="configuration-toggle">
                                    <input
                                        type="checkbox"
                                        checked={item.active}
                                        onChange={(event) => updateDraft(
                                            item.field,
                                            event.target.checked
                                        )}
                                    />
                                    <span>
                                        {item.active ? "ACTIVE" : "DISABLED"}
                                    </span>
                                </label>
                            ) : (
                                <span className="configuration-active">
                                    ACTIVE
                                </span>
                            )}
                        </div>
                    ))}
                </div>
            </section>

            <section className="configuration-section">
                <div className="configuration-section-header">
                    <div>
                        <span className="page-label">ROUTING</span>
                        <h2>Registered endpoints</h2>
                    </div>
                </div>

                <div className="configuration-list">
                    {endpoints.map((endpoint) => (
                        <div className="configuration-row" key={endpoint.path}>
                            <div className="configuration-row-content">
                                <strong>{endpoint.name}</strong>
                            </div>

                            <code>{endpoint.path}</code>
                        </div>
                    ))}
                </div>
            </section>
        </main>
    );
}

export default Configuration;