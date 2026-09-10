import { useEffect, useRef, useState } from "react";

import Chart from "chart.js/auto";

import * as api from "../services/api";

import type { Metrics } from "../types/api";

import "../styles/lineGraph.css";

const STORAGE_KEY = "forgehttp_metrics_history";

function LineGraph() {
    const chartRef = useRef<HTMLCanvasElement | null>(null);
    const chartInstance = useRef<Chart | null>(null);

    const [metricsHistory, setMetricsHistory] = useState<Metrics[]>(() => {
        const stored = localStorage.getItem(STORAGE_KEY);

        if (!stored) {
            return [];
        }

        try {
            return JSON.parse(stored);
        } catch {
            return [];
        }
    });

    const [sampleCount, setSampleCount] = useState(20);

    /*
     * Collect metrics every 5 seconds.
     */
    useEffect(() => {
        async function loadMetrics() {
            try {
                const metrics = await api.getMetrics();

                setMetricsHistory((previous) => {
                    const updated = [...previous, metrics];

                    localStorage.setItem(
                        STORAGE_KEY,
                        JSON.stringify(updated)
                    );

                    return updated;
                });
            } catch {
                // Metrics unavailable.
            }
        }

        loadMetrics();

        const interval = setInterval(loadMetrics, 5000);

        return () => clearInterval(interval);
    }, []);

    /*
     * Create Chart.js once.
     */
    useEffect(() => {
        if (!chartRef.current) {
            return;
        }

        chartInstance.current = new Chart(chartRef.current, {
            type: "line",

            data: {
                labels: [],
                datasets: [
                    {
                        label: "Requests",
                        data: [],

                        borderWidth: 2,
                        tension: 0.3,
                        pointRadius: 3,

                        borderColor: "#33ff66",
                        pointBackgroundColor: "#33ff66",
                        pointBorderColor: "#33ff66",
                    },
                ],
            },

            options: {
                responsive: true,
                maintainAspectRatio: false,

                animation: false,

                plugins: {
                    legend: {
                        display: true,
                    },
                },

                scales: {
                    x: {
                        ticks: {
                            maxTicksLimit: 8,
                        },

                        grid: {
                            color: "rgba(216, 230, 216, 0.08)",
                        },
                    },

                    y: {
                        beginAtZero: true,

                        grid: {
                            color: "rgba(216, 230, 216, 0.08)",
                        },
                    },
                },
            },
        });

        return () => {
            chartInstance.current?.destroy();
            chartInstance.current = null;
        };
    }, []);

    /*
     * Update the existing chart.
     */
    useEffect(() => {
        if (!chartInstance.current) {
            return;
        }

        const samples = metricsHistory.slice(-sampleCount);

        chartInstance.current.data.labels = samples.map((_, index) => {
            return `${(index + 1) * 5}s`;
        });

        chartInstance.current.data.datasets[0].data =
            samples.map((metrics) => metrics.requests);

        chartInstance.current.update("none");
    }, [metricsHistory, sampleCount]);

    return (
        <div className="line-graph">
            <div className="line-graph-header">
                <div>
                    <span className="page-label">
                        REQUEST ACTIVITY
                    </span>

                    <h2>Requests over time</h2>
                </div>

                <select
                    value={sampleCount}
                    onChange={(event) =>
                        setSampleCount(Number(event.target.value))
                    }
                >
                    <option value={20}>
                        Last 20 samples
                    </option>

                    <option value={50}>
                        Last 50 samples
                    </option>

                    <option value={100}>
                        Last 100 samples
                    </option>
                </select>
            </div>

            <div className="line-graph-chart">
                <canvas ref={chartRef}></canvas>
            </div>
        </div>
    );
}

export default LineGraph;