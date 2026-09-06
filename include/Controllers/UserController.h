#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H

#include "HttpResponse.h"
#include "HttpRequest.h"

// Backs the example JSON API described in PLAN.md
class UserController {
    public:
        HttpResponse getUser(HttpRequest req);
        HttpResponse createUser(HttpResponse req);
};

#endif