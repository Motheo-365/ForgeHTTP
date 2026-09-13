import {
    createContext,
    useContext,
    useCallback,
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

const METRICS_STORAGE_KEY = "forgehttp-metrics";

interface PersistedMetrics {
    metrics: Metrics;
    backendRequests: number;
    backendErrors: number;
}

function readPersistedMetrics(): PersistedMetrics | null {
    try {
        const stored = window.localStorage.getItem(METRICS_STORAGE_KEY);
        const parsed = stored ? JSON.parse(stored) : null;

        if (
            !parsed ||
            !parsed.metrics ||
            typeof parsed.backendRequests !== "number" ||
            typeof parsed.backendErrors !== "number"
        ) {
            return null;
        }

        return parsed as PersistedMetrics;
    } catch {
        return null;
    }
}

interface MetricsContextType {
    metrics: Metrics | null;
    metricsHistory: MetricsSample[];
    requestHistory: RequestHistoryEntry[];
    loading: boolean;
    error: string | null;
    refreshMetrics: () => Promise<void>;
}

const MetricsContext = createContext<MetricsContextType | undefined>(
    undefined
);

export function MetricsProvider({ children }: { children: ReactNode }) {
    const [metrics, setMetrics] = useState<Metrics | null>(
        () => readPersistedMetrics()?.metrics ?? null
    );
    const [metricsHistory, setMetricsHistory] = useState<MetricsSample[]>([]);
    const [requestHistory, setRequestHistory] = useState<RequestHistoryEntry[]>(() => {
        try {
            const stored = window.localStorage.getItem("forgehttp-request-history");
            const parsed = stored ? JSON.parse(stored) : null;
            return Array.isArray(parsed) ? parsed as RequestHistoryEntry[] : [];
        } catch {
            return [];
        }
    });
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState<string | null>(null);

    const refreshMetrics = useCallback(async (): Promise<void> => {
        try {
            const [metricsData, requestData] = await Promise.all([
                api.getMetrics(true),
                api.getRequestHistory(true)
            ]);

            const persisted = readPersistedMetrics();
            const requestDelta = persisted
                ? metricsData.requests >= persisted.backendRequests
                    ? metricsData.requests - persisted.backendRequests
                    : metricsData.requests
                : metricsData.requests;
            const errorDelta = persisted
                ? metricsData.errors >= persisted.backendErrors
                    ? metricsData.errors - persisted.backendErrors
                    : metricsData.errors
                : metricsData.errors;
            const mergedMetrics: Metrics = {
                ...metricsData,
                requests: (persisted?.metrics.requests ?? 0) + requestDelta,
                errors: (persisted?.metrics.errors ?? 0) + errorDelta
            };

            setMetrics(mergedMetrics);
            window.localStorage.setItem(
                METRICS_STORAGE_KEY,
                JSON.stringify({
                    metrics: mergedMetrics,
                    backendRequests: metricsData.requests,
                    backendErrors: metricsData.errors
                } satisfies PersistedMetrics)
            );

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

            setRequestHistory((previous) => {
                const entries = new Map<string, RequestHistoryEntry>();

                for (const request of [...requestData, ...previous]) {
                    const key = [
                        request.time,
                        request.method,
                        request.path,
                        request.status,
                        request.duration_ms
                    ].join("|");

                    entries.set(key, request);
                }

                const cutoff = Date.now() - 30 * 24 * 60 * 60 * 1000;
                const merged = Array.from(entries.values())
                    .filter((request) => {
                        const timestamp = Date.parse(request.time);
                        return Number.isNaN(timestamp) || timestamp >= cutoff;
                    })
                    .sort(
                        (left, right) =>
                            Date.parse(right.time) - Date.parse(left.time)
                    );

                window.localStorage.setItem(
                    "forgehttp-request-history",
                    JSON.stringify(merged)
                );

                return merged;
            });
            setError(null);
        } catch {
            setError("Unable to retrieve server metrics.");
        } finally {
            setLoading(false);
        }
    }, []);

    useEffect(() => {
        const initialRefresh = window.setTimeout(() => {
            void refreshMetrics();
        }, 0);

        const interval = window.setInterval(() => {
            void refreshMetrics();
        }, 10000);

        return () => {
            window.clearTimeout(initialRefresh);
            window.clearInterval(interval);
        };
    }, [refreshMetrics]);

    return (
        <MetricsContext.Provider
            value={{
                metrics,
                metricsHistory,
                requestHistory,
                loading,
                error,
                refreshMetrics
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