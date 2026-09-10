import type {
    HealthResponse,
    User,
    CreateUserRequest
} from "../types/api";

const DEFAULT_API_URL = "https://forgehttp.onrender.com"

export const API_URL = (
    import.meta.env.VITE_API_URL || DEFAULT_API_URL
).replace(/\/$/, "");

// CRUD: GET, POST, PUT, DELETE
export async function getHealth (): Promise<HealthResponse> {
    const response = await fetch(`${API_URL}/health`);

    if (!response.ok) {
        throw new Error(`Health check failed: ${response.status}`);
    }

    return response.json();
}

export async function getUsers (): Promise<User[]> {
    const response = await fetch(`${API_URL}/api/users`, {
        headers: {
            Authorization: "Bearer dashboard"
        }
    });

    if (!response.ok) {
        throw new Error(`Failed to fetch users: ${response.status}`);
    }

    return response.json();
}

export async function createUser (user: CreateUserRequest): Promise<User> {
    const response = await fetch(`${API_URL}/api/users`, {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
            Authorization: "Bearer dashboard"
        },
        body: JSON.stringify(user)
    });

    if (!response.ok) {
        throw new Error(`Failed to create user: ${response.status}`);
    }

    return response.json();
}