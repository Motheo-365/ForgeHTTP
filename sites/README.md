# Greenline Calculator

Greenline is a small standalone calculator website that demonstrates using
ForgeHTTP as an API server. The browser sends arithmetic requests to
`POST https://forgehttp.onrender.com/api/calculate`; the C++ server validates the
inputs, calculates the result, and returns JSON. This is intentionally different
from the ForgeHTTP monitoring dashboard.

## Preview

Open `index.html` in a browser or serve this folder with a static-file host.
The API must have CORS enabled and be reachable at the configured API origin in
`app.js`.

The calculator needs a ForgeHTTP backend build that includes `POST /api/calculate`.
Redeploy the backend before trying the calculator against the hosted API, then
publish this `sites/` directory on a static host.

## API

Request:

```json
{"left":24,"operator":"+","right":6}
```

Response:

```json
{"left":24.0,"operator":"+","right":6.0,"result":30.0}
```

Supported operators are `+`, `-`, `*`, and `/`. Invalid operands, unsupported
operators, division by zero, and non-finite results receive a `400` response.
The calculator endpoint is public and does not change user or configuration data.
Like other non-internal API calls, calculations appear in server metrics and request
history. User operations and configuration updates remain protected by
`FORGEHTTP_API_TOKEN`.
