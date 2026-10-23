import type {
    HealthResponse,
    Metrics,
    User,
    CreateUserRequest,
    RequestHistoryEntry,
    ServerConfiguration
} from "../types/api";

const API_TOKEN_STORAGE_KEY = "forgehttp-api-token";
let API_TOKEN = window.sessionStorage.getItem(API_TOKEN_STORAGE_KEY) ?? "";

export const API_URL = "https://forgehttp.onrender.com";

export function getApiToken(): string {
    return API_TOKEN;
}

export function hasApiToken(): boolean {
    return API_TOKEN.length > 0;
}

function normalizedToken(token: string): string {
    return token.trim().replace(/^Bearer\s+/i, "");
}

export async function validateAdminToken(token: string): Promise<void> {
    const normalized = normalizedToken(token);
    if (!normalized) {
        throw new Error("Enter the admin token to sign in.");
    }

    const response = await fetch(`${API_URL}/api/users`, {
        headers: { Authorization: `Bearer ${normalized}` }
    });

    if (response.status === 401) {
        throw new Error("That admin token was rejected. Check the token and try again.");
    }
    if (response.status === 503) {
        throw new Error("Admin authentication is not configured on the server.");
    }
    if (!response.ok) {
        throw new Error(`Admin sign-in failed: ${response.status}`);
    }
}

export function setApiToken(token: string): void {
    API_TOKEN = normalizedToken(token);

    if (API_TOKEN) {
        window.sessionStorage.setItem(API_TOKEN_STORAGE_KEY, API_TOKEN);
    } else {
        window.sessionStorage.removeItem(API_TOKEN_STORAGE_KEY);
    }

    window.dispatchEvent(new Event("forgehttp-api-token-change"));
}

function authorizedHeaders(json = false): HeadersInit {
    if (!API_TOKEN) {
        throw new Error("Enter the API bearer token in the sidebar.");
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
        headers: internal ? { "X-ForgeHTTP-Internal": "true" } : undefined
    });

    if (!response.ok) {
        throw new Error(`Failed to fetch request history: ${response.status}`);
    }

    return response.json();
}

export async function getConfiguration(): Promise<ServerConfiguration> {
    const response = await fetch(`${API_URL}/api/config`);

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

    window.dispatchEvent(new Event("forgehttp-config-updated"));
    return data as ServerConfiguration;
}
