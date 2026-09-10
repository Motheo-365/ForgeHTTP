import {
    createContext,
    useContext,
    useEffect,
    useState,
    type ReactNode
} from "react";

import * as api from "../services/api";
import type {
    Metrics,
    RequestHistoryEntry
} from "../types/api";

interface MetricsContextType {
    metrics: Metrics | null;
    metricsHistory: Metrics[];
    requestHistory: RequestHistoryEntry[];
    loading: boolean;
    error: string | null;
}

const MetricsContext = createContext<MetricsContextType | undefined>(
    undefined
);

export function MetricsProvider({ children }: { children: ReactNode }) {
    const [metrics, setMetrics] = useState<Metrics | null>(null);
    const [metricsHistory, setMetricsHistory] = useState<Metrics[]>([]);
    const [requestHistory, setRequestHistory] = useState<RequestHistoryEntry[]>([]);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState<string | null>(null);

    useEffect(() => {
        async function loadMetrics() {
            try {
                const [metricsData, requestData] = await Promise.all([
                    api.getMetrics(),
                    api.getRequestHistory()
                ]);

                setMetrics(metricsData);

                setMetricsHistory((previous) => [
                    ...previous,
                    metricsData
                ]);

                setRequestHistory(requestData);

                setError(null);
            } catch {
                setError("Unable to retrieve server metrics.");
            } finally {
                setLoading(false);
            }
        }

        loadMetrics();

        const interval = setInterval(loadMetrics, 5000);

        return () => clearInterval(interval);
    }, []);

    return (
        <MetricsContext.Provider
            value={{
                metrics,
                metricsHistory,
                requestHistory,
                loading,
                error
            }}
        >
            {children}
        </MetricsContext.Provider>
    );
}

export function useMetrics() {
    const context = useContext(MetricsContext);

    if (!context) {
        throw new Error(
            "useMetrics must be used inside MetricsProvider"
        );
    }

    return context;
}