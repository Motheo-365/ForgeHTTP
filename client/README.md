# ForgeHTTP Dashboard

The Vite dashboard defaults to the hosted API at `https://forgehttp.onrender.com`. It reads its API origin and bearer token from environment variables.

From the repository root, start the backend with a demo token:

```bash
FORGEHTTP_API_TOKEN=local-demo-token PORT=8086 ./backend/build/forge
```

Configure and start the client:

```bash
cd client
cp .env.example .env.local
npm install
npm run dev
```

For local development, set `VITE_API_URL=http://localhost:8086` and
`VITE_API_TOKEN=local-demo-token` in `client/.env.local` after copying the example.
To connect to the hosted API, leave `VITE_API_URL` at its default and configure
`VITE_API_TOKEN` with a token accepted by that server if you need its protected `/api/*`
endpoints.

`VITE_API_TOKEN` must match the backend's `FORGEHTTP_API_TOKEN`. Vite bundles this token into browser code, so use only a disposable demo token—not a production secret.
