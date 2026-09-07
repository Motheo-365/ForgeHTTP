#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H

#include "HTTP/HttpResponse.h"
#include "HTTP/HttpRequest.h"

#include <string>
#include <vector>

// In memory User structure
struct User {
    int id;
    std::string name;
};

// Backs the example JSON API described in PLAN.md
class UserController {
    public:
        UserController();

        // Returns a 200 JSON array of users. registered against GET /api/users
        HttpResponse getUsers(const HttpRequest& req);

        // Parses the request body, validates it, adds a new user, and returns a 201 response with the created record (or 400 resposne if teh body is invalid). 
        // Registered against POST /api/users
        HttpResponse createUser(const HttpRequest& req);

    private:
        std::vector<User> users;
        int nextId = 1;
};

#endif