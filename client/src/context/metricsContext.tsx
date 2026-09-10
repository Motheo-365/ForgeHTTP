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
    MetricsSample,
    RequestHistoryEntry
} from "../types/api";

interface MetricsContextType {
    metrics: Metrics | null;
    metricsHistory: MetricsSample[];
    requestHistory: RequestHistoryEntry[];
    loading: boolean;
    error: string | null;
}

const MetricsContext = createContext<MetricsContextType | undefined>(
    undefined
);

export function MetricsProvider({ children }: { children: ReactNode }) {
    const [metrics, setMetrics] = useState<Metrics | null>(null);
    const [metricsHistory, setMetricsHistory] = useState<MetricsSample[]>([]);
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

                const sample: MetricsSample = {
                    ...metricsData,
                    time: new Date().toLocaleTimeString([], {
                        hour: "2-digit",
                        minute: "2-digit",
                        second: "2-digit"
                    })
                };

                setMetricsHistory((previous) => [
                    ...previous,
                    sample
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