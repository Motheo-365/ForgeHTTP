export interface HealthResponse {
    status: string;
}

export interface Metrics {
    requests: number;
    active_connections: number;
    average_response_time_ms: number;
    errors: number;
    workers: number;
}

export interface MetricsSample extends Metrics {
    time: string;
}

export interface RequestHistoryEntry {
    time: string;
    method: string;
    path: string;
    status: number;
    duration_ms: number;
}

export interface User {
    id: number;
    name: string;
}

export interface CreateUserRequest {
    name: string;
}

export interface ServerConfiguration {
    port: number;
    workerThreads: number;
    rateLimit: number;
    requestHistoryLimit: number;
    corsEnabled: boolean;
    authenticationEnabled: boolean;
    loggingEnabled: boolean;
}