import { useEffect, useRef, useState } from "react";

import {
    getHealth,
    getMetrics,
    getRequestHistory,
    getConfiguration
} from "../services/api";
import { useMetrics } from "../context/metricsContext";

import "../styles/terminal.css";

function Terminal() {
    const [history, setHistory] = useState([
        "ForgeHTTP command console",
        'Type "help" to see available ForgeHTTP commands.',
        ""
    ]);

    const [command, setCommand] = useState("");
    const { refreshMetrics } = useMetrics();
    const inputRef = useRef<HTMLInputElement>(null);
    const terminalRef = useRef<HTMLDivElement>(null);

    useEffect(() => {
        inputRef.current?.focus();
    }, []);

    useEffect(() => {
        terminalRef.current?.scrollTo({
            top: terminalRef.current.scrollHeight,
            behavior: "smooth"
        });
    }, [history]);

    async function executeCommand(input: string) {
        const trimmed = input.trim();
        const lower = trimmed.toLowerCase();

        if (!trimmed) {
            setHistory((previous) => [...previous, ""]);
            return;
        }

        setHistory((previous) => [
            ...previous,
            `forgehttp@server:~$ ${trimmed}`
        ]);

        try {
            switch (lower) {
                case "help":
                    setHistory((previous) => [
                        ...previous,
                        "",
                        "ForgeHTTP commands:",
                        "",
                        "  help       Show available commands",
                        "  health     Check server health",
                        "  metrics    Show server metrics",
                        "  requests   Show recent requests",
                        "  routes     Show registered routes",
                        "  config     Show server configuration",
                        "  clear      Clear terminal output",
                        "",
                        "This console operates through the ForgeHTTP",
                        "dashboard API. It is not a system shell.",
                        ""
                    ]);
                    break;

                case "health": {
                    const data = await getHealth();

                    setHistory((previous) => [
                        ...previous,
                        "",
                        "SERVER HEALTH",
                        "-------------",
                        `STATUS      ${data.status?.toUpperCase() ?? "UNKNOWN"}`,
                        "SERVER      ForgeHTTP",
                        "PROTOCOL    HTTP/1.1",
                        ""
                    ]);
                    await refreshMetrics();
                    break;
                }

                case "metrics": {
                    const data = await getMetrics();

                    setHistory((previous) => [
                        ...previous,
                        "",
                        "SERVER METRICS",
                        "--------------",
                        `REQUESTS             ${data.requests}`,
                        `ACTIVE CONNECTIONS   ${data.active_connections}`,
                        `AVG RESPONSE TIME    ${data.average_response_time_ms} ms`,
                        `ERRORS               ${data.errors}`,
                        `WORKERS              ${data.workers}`,
                        ""
                    ]);
                    await refreshMetrics();
                    break;
                }

                case "requests": {
                    const data = await getRequestHistory();

                    setHistory((previous) => [
                        ...previous,
                        "",
                        "RECENT REQUESTS",
                        "---------------",
                        ...data.slice(0, 10).map(
                            (request) =>
                                `${request.time}  ${request.method.padEnd(6)} ${request.path}  ${request.status}  ${request.duration_ms} ms`
                        ),
                        ""
                    ]);
                    await refreshMetrics();
                    break;
                }

                case "routes":
                    setHistory((previous) => [
                        ...previous,
                        "",
                        "REGISTERED ROUTES",
                        "-----------------",
                        "GET    /health",
                        "GET    /metrics",
                        "GET    /api/users",
                        "POST   /api/users",
                        "GET    /api/requests",
                        "GET    /api/config",
                        "PUT    /api/config",
                        ""
                    ]);
                    break;

                case "config": {
                    const data = await getConfiguration();

                    setHistory((previous) => [
                        ...previous,
                        "",
                        "SERVER CONFIGURATION",
                        "--------------------",
                        `PORT                    ${data.port}`,
                        `WORKER THREADS          ${data.workerThreads}`,
                        `RATE LIMIT              ${data.rateLimit}`,
                        `REQUEST HISTORY LIMIT   ${data.requestHistoryLimit}`,
                        `CORS                    ${data.corsEnabled ? "ON" : "OFF"}`,
                        `AUTHENTICATION          ${data.authenticationEnabled ? "ON" : "OFF"}`,
                        `LOGGING                 ${data.loggingEnabled ? "ON" : "OFF"}`,
                        ""
                    ]);
                    await refreshMetrics();
                    break;
                }

                case "clear":
                    setHistory([]);
                    break;

                default:
                    setHistory((previous) => [
                        ...previous,
                        "",
                        `Unknown ForgeHTTP command: ${trimmed}`,
                        'Type "help" for available commands.',
                        ""
                    ]);
            }
        } catch (error) {
            setHistory((previous) => [
                ...previous,
                "",
                `ERROR    ${
                    error instanceof Error
                        ? error.message
                        : String(error)
                }`,
                ""
            ]);
        }
    }

    function handleSubmit(event: { preventDefault: () => void; }) {
        event.preventDefault();

        executeCommand(command);
        setCommand("");
    }

    function handleTerminalClick() {
        inputRef.current?.focus();
    }

    return (
        <main className="terminal-page">
            <header className="terminal-page-header">
                <div>
                    <span className="page-label">DEVELOPER CONSOLE</span>
                    <h1>Terminal</h1>
                    <p>
                        Inspect the running ForgeHTTP server through
                        the dashboard command console.
                    </p>
                </div>

                <button
                    className="terminal-clear-button"
                    onClick={() => setHistory([])}
                >
                    CLEAR
                </button>
            </header>

            <section
                className="terminal-window"
                onClick={handleTerminalClick}
            >
                <div className="terminal-window-header">
                    <span>FORGEHTTP CONSOLE</span>

                    <span className="terminal-status">
                        ● CONNECTED
                    </span>
                </div>

                <div
                    className="terminal-output"
                    ref={terminalRef}
                >
                    {history.map((line, index) => (
                        <div
                            className={
                                line.startsWith("forgehttp@server:~$")
                                    ? "terminal-line terminal-command"
                                    : "terminal-line"
                            }
                            key={index}
                        >
                            {line || "\u00A0"}
                        </div>
                    ))}

                    <form
                        className="terminal-input-line"
                        onSubmit={handleSubmit}
                    >
                        <span>forgehttp@server:~$</span>

                        <input
                            ref={inputRef}
                            value={command}
                            onChange={(event) =>
                                setCommand(event.target.value)
                            }
                            autoComplete="off"
                            spellCheck="false"
                            aria-label="ForgeHTTP command"
                        />

                        <span className="terminal-cursor">_</span>
                    </form>
                </div>
            </section>
        </main>
    );
}

export default Terminal;