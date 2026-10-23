import { useState, type FormEvent } from "react";

import Nav from "./nav";
import { useMetrics } from "../context/metricsContext";
import * as api from "../services/api";

import "../styles/sidebar.css";

function Sidebar() {
    const [token, setToken] = useState(() => api.getApiToken());
    const [message, setMessage] = useState("");
    const { refreshMetrics } = useMetrics();

    function saveToken(event: FormEvent<HTMLFormElement>): void {
        event.preventDefault();
        api.setApiToken(token);
        setToken(api.getApiToken());
        setMessage(token.trim() ? "Token saved for this tab." : "Token removed.");

        if (token.trim()) {
            void refreshMetrics();
        }
    }

    return (
        <aside className="sidebar">
            <div className="sidebar-logo">
                <span>FORGEHTTP</span>
                <small>SERVER CONTROL</small>
            </div>

            <Nav />

            <form className="sidebar-api-token" onSubmit={saveToken}>
                <label className="nav-label" htmlFor="api-token">
                    ADMIN ACCESS
                </label>
                <input
                    id="api-token"
                    type="password"
                    autoComplete="off"
                    spellCheck={false}
                    value={token}
                    onChange={(event) => {
                        setToken(event.target.value);
                        setMessage("");
                    }}
                    placeholder="Admin token (writes)"
                />
                <button type="submit">Save token</button>
                {message && <small role="status">{message}</small>}
            </form>
        </aside>
    );
}

export default Sidebar;
