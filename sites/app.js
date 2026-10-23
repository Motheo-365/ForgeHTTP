const API_ORIGIN = "https://forgehttp.onrender.com";
const CALCULATE_ENDPOINT = `${API_ORIGIN}/api/calculate`;

const form = document.querySelector("#calculator-form");
const leftInput = document.querySelector("#left-value");
const rightInput = document.querySelector("#right-value");
const operatorInput = document.querySelector("#operator");
const calculateButton = document.querySelector("#calculate-button");
const calculateLabel = document.querySelector("#calculate-label");
const resultValue = document.querySelector("#result-value");
const resultDetail = document.querySelector("#result-detail");
const formError = document.querySelector("#form-error");
const serverStatus = document.querySelector("#server-status");
const serverLight = document.querySelector("#server-light");
const requestPreview = document.querySelector("#request-json");
const responsePreview = document.querySelector("#response-json");
const responseCode = document.querySelector("#response-code");

const operatorSymbols = {
    "+": "+",
    "-": "−",
    "*": "×",
    "/": "÷"
};

function currentPayload() {
    return {
        left: Number(leftInput.value || 24),
        operator: operatorInput.value,
        right: Number(rightInput.value || 6)
    };
}

function updateRequestPreview() {
    requestPreview.textContent = JSON.stringify(currentPayload(), null, 2);
}

function setServerStatus(online, text) {
    serverLight.classList.toggle("is-online", online);
    serverLight.classList.toggle("is-offline", !online);
    serverStatus.textContent = text;
}

async function checkServer() {
    try {
        const response = await fetch(`${API_ORIGIN}/health`, { cache: "no-store" });
        if (!response.ok) throw new Error(`HTTP ${response.status}`);

        const health = await response.json();
        if (health.status !== "ok") throw new Error("Unexpected health response");

        setServerStatus(true, "FORGEHTTP ONLINE");
    } catch {
        setServerStatus(false, "FORGEHTTP UNAVAILABLE");
    }
}

async function calculate(event) {
    event.preventDefault();
    formError.hidden = true;
    calculateButton.disabled = true;
    calculateLabel.textContent = "SENDING REQUEST...";
    responseCode.className = "response-code";
    responseCode.textContent = "IN FLIGHT";

    const payload = currentPayload();
    requestPreview.textContent = JSON.stringify(payload, null, 2);
    const startedAt = performance.now();

    try {
        if (!Number.isFinite(payload.left) || !Number.isFinite(payload.right)) {
            throw new Error("Enter finite numbers for both operands.");
        }

        const response = await fetch(CALCULATE_ENDPOINT, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(payload)
        });

        const data = await response.json();
        const elapsed = (performance.now() - startedAt).toFixed(1);
        responsePreview.textContent = JSON.stringify(data, null, 2);
        responseCode.textContent = `${response.status} ${response.ok ? "OK" : "ERROR"}`;
        responseCode.className = `response-code ${response.ok ? "is-success" : "is-error"}`;

        if (!response.ok) {
            throw new Error(data.error || `The API returned HTTP ${response.status}.`);
        }

        const expression = `${payload.left} ${operatorSymbols[payload.operator]} ${payload.right}`;
        resultValue.textContent = String(data.result);
        resultDetail.textContent = `${expression} · ForgeHTTP response in ${elapsed} ms`;
        setServerStatus(true, "FORGEHTTP ONLINE");
    } catch (error) {
        const message = error instanceof Error
            ? error.message
            : "Unable to calculate with ForgeHTTP.";
        formError.textContent = message;
        formError.hidden = false;
        resultValue.textContent = "No result";
        resultDetail.textContent = "The server did not return a successful calculation.";
        responseCode.textContent = "REQUEST FAILED";
        responseCode.className = "response-code is-error";
        if (error instanceof TypeError) {
            setServerStatus(false, "FORGEHTTP UNAVAILABLE");
        }
    } finally {
        calculateButton.disabled = false;
        calculateLabel.textContent = "CALCULATE WITH FORGEHTTP";
    }
}

form.addEventListener("submit", calculate);
leftInput.addEventListener("input", updateRequestPreview);
rightInput.addEventListener("input", updateRequestPreview);
operatorInput.addEventListener("change", updateRequestPreview);
void checkServer();
