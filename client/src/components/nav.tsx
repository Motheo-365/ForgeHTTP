import { NavLink } from "react-router-dom";

import "../styles/nav.css";

function Nav() {
    return (
        <nav className="nav">
            <div className="nav-section">
                <span className="nav-label">GETTING STARTED</span>

                <NavLink to="/introduction">Introduction</NavLink>
                <NavLink to="/dashboard">Dashboard</NavLink>
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