import { useEffect, useRef, useState } from "react";

import Chart from "chart.js/auto";

import { useMetrics } from "../context/metricsContext";

import "../styles/lineGraph.css";

function LineGraph() {
    const chartRef = useRef<HTMLCanvasElement | null>(null);
    const chartInstance = useRef<Chart | null>(null);
    const { requestHistory } = useMetrics();

    const [sampleCount, setSampleCount] = useState(20);

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
                            autoSkip: false,
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

        const samples = requestHistory
            .slice(0, sampleCount)
            .reverse();

        chartInstance.current.data.labels = samples.map(
            (request) => request.time
        );

        chartInstance.current.data.datasets[0].data =
            samples.map(
                (_, index) => requestHistory.length - samples.length + index + 1
            );

        chartInstance.current.update("none");
    }, [requestHistory, sampleCount]);

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
                        Last 20 requests
                    </option>

                    <option value={50}>
                        Last 50 requests
                    </option>

                    <option value={100}>
                        Last 100 requests
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