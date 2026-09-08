import { useEffect, useRef } from 'react';
import Chart from 'chart.js/auto';

const data = [
    { year: 2010, count: 10 },
    { year: 2011, count: 20 },
    { year: 2012, count: 15 },
    { year: 2013, count: 25 },
    { year: 2014, count: 22 },
    { year: 2015, count: 30 },
    { year: 2016, count: 28 },
];

function LineGraph() {
    const chartRef = useRef<HTMLCanvasElement | null>(null);

    useEffect(() => {
        if (!chartRef.current) return;

        const chart = new Chart(chartRef.current, {
            type: 'line',
            data: {
                labels: data.map(row => row.year),
                datasets: [
                    {
                        label: 'Acquisitions by year',
                        data: data.map(row => row.count),
                        borderWidth: 2,
                        tension: 0.4,
                    },
                ],
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
            },
        });

        return () => {
            chart.destroy();
        };
    }, []);

    return (
        <>
            <select name="duration">
                <option>Last 24 hours</option>
                <option>Last 7 days</option>
                <option>Last month</option>
                <option>Last Year</option>
            </select>

            <div style={{ height: '300px' }}>
                <canvas ref={chartRef}></canvas>
            </div>
        </>
    );
}

export default LineGraph;