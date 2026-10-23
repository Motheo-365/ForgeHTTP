# ForgeHTTP Dashboard

The dashboard at `https://forgehttp.netlify.app/` connects to
`https://forgehttp.onrender.com`.

Start the client:

```bash
cd client
npm install
npm run dev
```

Metrics, request history, and configuration are publicly readable for the demo
dashboard. User operations and configuration changes require the server's
`FORGEHTTP_API_TOKEN`; enter that admin token in the sidebar when needed. It is kept
in the current browser tab's session storage and cleared when that tab's session ends.
Do not distribute the admin token to dashboard testers or commit it to source control.
