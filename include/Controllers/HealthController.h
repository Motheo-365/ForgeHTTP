#ifndef HEALTHCONTROLLER_H
#define HEALTHCONTROLLER_H

#include "HttpResponse.h"
#include "HttpRequest.h"

// Trivial liveness endpoint used to confirm the server is up.
class HealthController {
    public:
        HttpResponse getHealth(HttpRequest req);
}

#endif