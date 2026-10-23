import { useEffect, useRef, useState, type FormEvent } from "react";

import Nav from "./nav";
import * as api from "../services/api";

import "../styles/sidebar.css";

function Sidebar() {
    const [modalOpen, setModalOpen] = useState(false);
    const [token, setToken] = useState("");
    const [message, setMessage] = useState("");
    const [submitting, setSubmitting] = useState(false);
    const [signedIn, setSignedIn] = useState(api.hasApiToken());
    const tokenInputRef = useRef<HTMLInputElement>(null);

    useEffect(() => {
        if (modalOpen && !signedIn) {
            tokenInputRef.current?.focus();
        }
    }, [modalOpen, signedIn]);

    useEffect(() => {
        function closeOnEscape(event: KeyboardEvent): void {
            if (event.key === "Escape") {
                setModalOpen(false);
            }
        }

        window.addEventListener("keydown", closeOnEscape);
        return () => window.removeEventListener("keydown", closeOnEscape);
    }, []);

    async function signIn(event: FormEvent<HTMLFormElement>): Promise<void> {
        event.preventDefault();
        setSubmitting(true);
        setMessage("");

        try {
            await api.validateAdminToken(token);
            api.setApiToken(token);
            setSignedIn(true);
            setToken("");
            setMessage("Admin access verified for this tab.");
        } catch (signInError) {
            setMessage(
                signInError instanceof Error
                    ? signInError.message
                    : "Unable to verify admin access."
            );
        } finally {
            setSubmitting(false);
        }
    }

    function signOut(): void {
        api.setApiToken("");
        setSignedIn(false);
        setToken("");
        setMessage("Signed out.");
    }

    return (
        <aside className="sidebar">
            <div className="sidebar-logo">
                <span>FORGEHTTP</span>
                <small>SERVER CONTROL</small>
            </div>

            <Nav />

            <div className="sidebar-admin">
                <button
                    className="sidebar-admin-button"
                    type="button"
                    title={signedIn ? "Admin session — manage access" : "Admin sign in — required for changes"}
                    aria-label={signedIn ? "Admin session — manage access" : "Admin sign in — required for changes"}
                    aria-haspopup="dialog"
                    aria-expanded={modalOpen}
                    onClick={() => {
                        setMessage("");
                        setModalOpen(true);
                    }}
                >
                    <svg viewBox="0 0 24 24" aria-hidden="true">
                        <path d="M20 21a8 8 0 0 0-16 0" />
                        <circle cx="12" cy="8" r="4" />
                    </svg>
                    <span>{signedIn ? "ADMIN SIGNED IN" : "ADMIN SIGN IN"}</span>
                    <span className={`sidebar-admin-status${signedIn ? " is-active" : ""}`} />
                </button>
            </div>

            {modalOpen && (
                <div
                    className="admin-modal-backdrop"
                    onMouseDown={(event) => {
                        if (event.target === event.currentTarget) {
                            setModalOpen(false);
                        }
                    }}
                >
                    <section
                        className="admin-modal"
                        role="dialog"
                        aria-modal="true"
                        aria-labelledby="admin-modal-title"
                    >
                        <button
                            className="admin-modal-close"
                            type="button"
                            aria-label="Close admin sign-in"
                            onClick={() => setModalOpen(false)}
                        >
                            ×
                        </button>
                        <span className="page-label">FORGEHTTP ACCESS</span>
                        <h2 id="admin-modal-title">
                            {signedIn ? "Admin session" : "Admin sign in"}
                        </h2>

                        {signedIn ? (
                            <>
                                <p className="admin-modal-description">
                                    Admin access is active in this browser tab.
                                    Protected changes can now be submitted.
                                </p>
                                {message && <p className="admin-modal-message" role="status">{message}</p>}
                                <button className="admin-modal-submit" type="button" onClick={signOut}>
                                    SIGN OUT
                                </button>
                            </>
                        ) : (
                            <form onSubmit={signIn}>
                                <label htmlFor="admin-token">ADMIN TOKEN</label>
                                <input
                                    ref={tokenInputRef}
                                    id="admin-token"
                                    type="password"
                                    autoComplete="current-password"
                                    spellCheck={false}
                                    value={token}
                                    onChange={(event) => {
                                        setToken(event.target.value);
                                        setMessage("");
                                    }}
                                    placeholder="Enter admin token"
                                    required
                                />
                                <p className="admin-modal-description">
                                    The token is verified by the server and kept only
                                    for this browser tab.
                                </p>
                                {message && <p className="admin-modal-message" role="alert">{message}</p>}
                                <button className="admin-modal-submit" type="submit" disabled={submitting}>
                                    {submitting ? "VERIFYING..." : "SIGN IN"}
                                </button>
                            </form>
                        )}
                    </section>
                </div>
            )}
        </aside>
    );
}

export default Sidebar;
