import "../styles/header.css";

function Header() {
    return (
        <header className="header">
            <div>
                <span className="header-path">FORGEHTTP</span>
            </div>

            <div className="server-status">
                <span className="status-dot"></span>
                <span>ONLINE</span>
            </div>
        </header>
    );
}

export default Header;