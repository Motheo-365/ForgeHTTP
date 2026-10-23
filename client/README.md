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
`FORGEHTTP_API_TOKEN`; use the user icon at the bottom of the sidebar to open the
admin sign-in modal and enter that token. It is verified by the server, kept in the
current browser tab's session storage, and cleared when you sign out or the tab session
ends. Do not distribute the admin token to dashboard testers or commit it to source
control. Dashboard request history is retained by the browser and is not trimmed by age.
