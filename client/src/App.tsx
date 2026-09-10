import { Navigate, BrowserRouter, Routes, Route } from "react-router-dom";

import Layout from "./components/layout";

import Introduction from "./pages/introduction";
import Dashboard from "./pages/dashboard";
import Requests from "./pages/requests";
import RoutesPage from "./pages/routes";
import Middleware from "./pages/middleware";
import Metrics from "./pages/metrics";
import Health from "./pages/health";
import Configuration from "./pages/configuration";

function App() {
    return (
        <BrowserRouter>
            <Routes>
                <Route element={<Layout />}>
                    <Route path="/" element={<Navigate to="/introduction" replace />} />                    <Route path="/introduction" element={<Introduction />} />
                    <Route path="/dashboard" element={<Dashboard />} />
                    <Route path="/requests" element={<Requests />} />
                    <Route path="/routes" element={<RoutesPage />} />
                    <Route path="/middleware" element={<Middleware />} />
                    <Route path="/metrics" element={<Metrics />} />
                    <Route path="/health" element={<Health />} />
                    <Route path="/configuration" element={<Configuration />} />
                </Route>
            </Routes>
        </BrowserRouter>
    );
}

export default App;