#ifndef HEALTHCONTROLLER_H
#define HEALTHCONTROLLER_H

#include "HTTP/HttpResponse.h"
#include "HTTP/HttpRequest.h"

// Trivial liveness endpoint used to confirm the server is up.
class HealthController {
    public:
        // returns a 200 JSON response confirming the server is running. registered by the Server against GET /health
        HttpResponse getHealth(const HttpRequest& req);
};

#endif