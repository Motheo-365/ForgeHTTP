import { Outlet } from "react-router-dom";

import Sidebar from "./sidebar";
import Header from "./header";

import "../styles/layout.css";

function Layout() {
    return (
        <div className="app">
            <Sidebar />

            <div className="main">
                <Header />

                <main className="content">
                    <Outlet />
                </main>
            </div>
        </div>
    );
}

export default Layout;