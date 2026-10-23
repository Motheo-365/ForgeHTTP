import { useEffect, useRef, useState } from "react";

import Chart from "chart.js/auto";

import { useMetrics } from "../context/metricsContext";
import {
    formatRequestTime,
    getRollingRequestActivity,
    filterRequestsByRange,
    REQUEST_RANGE_OPTIONS,
    type RequestRange
} from "../services/time";

import "../styles/lineGraph.css";

function LineGraph() {
    const chartRef = useRef<HTMLCanvasElement | null>(null);
    const chartInstance = useRef<Chart | null>(null);
    const { requestHistory } = useMetrics();

    const [range, setRange] = useState<RequestRange>("24h");

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
                        label: "Requests per minute",
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
                    tooltip: {
                        callbacks: {
                            title: (items) => {
                                return items[0]?.label ?? "";
                            },
                            label: (item) => {
                                return `${item.raw} requests in the last minute`;
                            },
                        },
                    },
                },

                scales: {
                    x: {
                        ticks: {
                            autoSkip: true,
                            maxTicksLimit: 8,
                            maxRotation: 45,
                            minRotation: 45,
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

        const samples = filterRequestsByRange(requestHistory, range)
            .reverse();

        const activity = getRollingRequestActivity(
            samples.map((request) => request.time)
        );

        chartInstance.current.data.labels = samples.map(
            (request) => formatRequestTime(request.time)
        );

        chartInstance.current.data.datasets[0].data = activity;

        chartInstance.current.update("none");
    }, [requestHistory, range]);

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
                    value={range}
                    onChange={(event) =>
                        setRange(event.target.value as RequestRange)
                    }
                >
                    {REQUEST_RANGE_OPTIONS.map((option) => (
                        <option key={option.value} value={option.value}>
                            {option.label}
                        </option>
                    ))}
                </select>
            </div>

            <div className="line-graph-chart">
                <canvas ref={chartRef}></canvas>
            </div>
        </div>
    );
}

export default LineGraph;