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

export interface User {
    id: number;
    name: string;
}

export interface CreateUserRequest {
    name: string;
}