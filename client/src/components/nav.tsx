import { useState } from "react";
import { Link, NavLink, useLocation } from "react-router-dom";

import "../styles/nav.css";

function Nav() {
    const [introductionOpen, setIntroductionOpen] = useState(true);
    const location = useLocation();

    const introductionActive =
        location.pathname === "/introduction";

    const isIntroductionSectionActive = (section: string) =>
        introductionActive && location.hash === `#${section}`;

    return (
        <nav className="nav">
            <div className="nav-section">
                <span className="nav-label">GETTING STARTED</span>

                <div className="nav-dropdown">
                    <button
                        className={`nav-dropdown-toggle ${
                            introductionActive ? "active" : ""
                        }`}
                        onClick={() =>
                            setIntroductionOpen(!introductionOpen)
                        }
                    >
                        <span>
                            <span className="nav-arrow">
                                {introductionOpen ? "▼" : "▶"}
                            </span>
                            Introduction
                        </span>
                    </button>

                    {introductionOpen && (
                        <div className="nav-subnav">
                            <Link
                                to="/introduction#overview"
                                className={isIntroductionSectionActive("overview") ? "active" : ""}
                                aria-current={isIntroductionSectionActive("overview") ? "page" : undefined}
                            >
                                Overview
                            </Link>

                            <Link
                                to="/introduction#request-lifecycle"
                                className={isIntroductionSectionActive("request-lifecycle") ? "active" : ""}
                                aria-current={isIntroductionSectionActive("request-lifecycle") ? "page" : undefined}
                            >
                                Request Lifecycle
                            </Link>

                            <Link
                                to="/introduction#dashboard"
                                className={isIntroductionSectionActive("dashboard") ? "active" : ""}
                                aria-current={isIntroductionSectionActive("dashboard") ? "page" : undefined}
                            >
                                Dashboard
                            </Link>

                            <Link
                                to="/introduction#quick-start"
                                className={isIntroductionSectionActive("quick-start") ? "active" : ""}
                                aria-current={isIntroductionSectionActive("quick-start") ? "page" : undefined}
                            >
                                Quick Start
                            </Link>
                        </div>
                    )}
                </div>

                <NavLink to="/dashboard">
                    Dashboard
                </NavLink>
            </div>

            <div className="nav-section">
                <span className="nav-label">SERVER</span>

                <NavLink to="/requests">Requests</NavLink>
                <NavLink to="/routes">Routes</NavLink>
                <NavLink to="/middleware">Middleware</NavLink>
                <NavLink to="/metrics">Metrics</NavLink>
                <NavLink to="/health">Health</NavLink>
                <NavLink to="/configuration">Configuration</NavLink>
            </div>
        </nav>
    );
}

export default Nav;