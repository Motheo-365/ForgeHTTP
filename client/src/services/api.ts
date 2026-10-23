import type {
    HealthResponse,
    Metrics,
    User,
    CreateUserRequest,
    RequestHistoryEntry,
    ServerConfiguration
} from "../types/api";

const DEFAULT_API_URL = "http://localhost:8086";
const API_TOKEN = import.meta.env.VITE_API_TOKEN || "";

export const API_URL = (
    import.meta.env.VITE_API_URL || DEFAULT_API_URL
).replace(/\/$/, "");

function authorizedHeaders(json = false): HeadersInit {
    if (!API_TOKEN) {
        throw new Error("Set VITE_API_TOKEN in the client environment.");
    }

    return {
        ...(json ? { "Content-Type": "application/json" } : {}),
        Authorization: `Bearer ${API_TOKEN}`
    };
}

export async function getHealth(): Promise<HealthResponse> {
    const response = await fetch(`${API_URL}/health`);

    if (!response.ok) {
        throw new Error(`Health check failed: ${response.status}`);
    }

    return response.json();
}

export async function getMetrics(internal = false): Promise<Metrics> {
    const response = await fetch(`${API_URL}/metrics`, {
        headers: internal ? { "X-ForgeHTTP-Internal": "true" } : undefined
    });

    if (!response.ok) {
        throw new Error(`Failed to fetch metrics: ${response.status}`);
    }

    return response.json();
}

export async function getUsers(): Promise<User[]> {
    const response = await fetch(`${API_URL}/api/users`, {
        headers: authorizedHeaders()
    });

    if (!response.ok) {
        throw new Error(`Failed to fetch users: ${response.status}`);
    }

    return response.json();
}

export async function createUser(user: CreateUserRequest): Promise<User> {
    const response = await fetch(`${API_URL}/api/users`, {
        method: "POST",
        headers: authorizedHeaders(true),
        body: JSON.stringify(user)
    });

    if (!response.ok) {
        throw new Error(`Failed to create user: ${response.status}`);
    }

    return response.json();
}

export async function getRequestHistory(internal = false): Promise<RequestHistoryEntry[]> {
    const response = await fetch(`${API_URL}/api/requests`, {
        headers: authorizedHeaders()
    });

    if (!response.ok) {
        throw new Error(`Failed to fetch request history: ${response.status}`);
    }

    return response.json();
}

export async function getConfiguration(): Promise<ServerConfiguration> {
    const response = await fetch(`${API_URL}/api/config`, {
        headers: authorizedHeaders()
    });

    if (!response.ok) {
        throw new Error(`Failed to fetch configuration: ${response.status}`);
    }

    return response.json();
}

export async function updateConfiguration(
    configuration: ServerConfiguration
): Promise<ServerConfiguration> {
    const response = await fetch(`${API_URL}/api/config`, {
        method: "PUT",
        headers: authorizedHeaders(true),
        body: JSON.stringify(configuration)
    });

    const data = await response.json().catch(() => null);

    if (!response.ok) {
        throw new Error(
            data?.error || `Failed to update configuration: ${response.status}`
        );
    }

    return data as ServerConfiguration;
}
