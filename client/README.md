# ForgeHTTP Dashboard

The Vite dashboard reads its API origin and demo bearer token from environment variables.

From the repository root, start the backend with a demo token:

```bash
FORGEHTTP_API_TOKEN=local-demo-token PORT=8086 ./backend/build/forge
```

In a second terminal, configure and start the client:

```bash
cd client
cp .env.example .env.local
npm install
npm run dev
```

`VITE_API_URL` defaults to `http://localhost:8086`. `VITE_API_TOKEN` must match the backend's `FORGEHTTP_API_TOKEN`.

Vite variables are bundled into browser code. Use only a disposable demo token here; this is not a secure way to distribute production credentials.
