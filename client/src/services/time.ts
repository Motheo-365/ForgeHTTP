export type RequestRange = "24h" | "7d" | "30d";

export const REQUEST_RANGE_OPTIONS: Array<{
    value: RequestRange;
    label: string;
}> = [
    { value: "24h", label: "Last 24 hours" },
    { value: "7d", label: "Last week" },
    { value: "30d", label: "Last month" }
];

export function filterRequestsByRange<T extends { time: string }>(
    requests: T[],
    range: RequestRange
): T[] {
    const duration = {
        "24h": 24 * 60 * 60 * 1000,
        "7d": 7 * 24 * 60 * 60 * 1000,
        "30d": 30 * 24 * 60 * 60 * 1000
    }[range];
    const cutoff = Date.now() - duration;

    return requests.filter((request) => {
        const timestamp = Date.parse(request.time);
        return !Number.isNaN(timestamp) && timestamp >= cutoff;
    });
}

export function parseRequestTime(value: string): Date | null {
    const date = new Date(value);

    return Number.isNaN(date.getTime()) ? null : date;
}

export function getRollingRequestActivity(times: string[]): number[] {
    const timestamps = times.map(
        (time) => parseRequestTime(time)?.getTime() ?? null
    );

    return timestamps.map((timestamp, index) => {
        if (timestamp === null) {
            return 1;
        }

        const windowStart = timestamp - 60_000;

        return timestamps
            .slice(0, index + 1)
            .filter(
                (candidate): candidate is number =>
                    candidate !== null &&
                    candidate >= windowStart &&
                    candidate <= timestamp
            )
            .length;
    });
}

export function formatRequestTime(value: string): string {
    const date = parseRequestTime(value);

    if (!date) {
        return value;
    }

    return date.toLocaleTimeString([], {
        hour: "2-digit",
        minute: "2-digit",
        second: "2-digit"
    });
}
