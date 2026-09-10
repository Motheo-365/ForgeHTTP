import Nav from "./nav";

import "../styles/sidebar.css";

function Sidebar() {
    return (
        <aside className="sidebar">
            <div className="sidebar-logo">
                <span>FORGEHTTP</span>
                <small>SERVER CONTROL</small>
            </div>

            <Nav />
        </aside>
    );
}

export default Sidebar;
