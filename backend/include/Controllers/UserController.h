#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H

#include "HTTP/HttpResponse.h"
#include "HTTP/HttpRequest.h"

#include <mutex>
#include <string>

struct sqlite3;

class UserController {
public:
    explicit UserController(const std::string& databasePath = {});
    ~UserController();

    UserController(const UserController&) = delete;
    UserController& operator=(const UserController&) = delete;

    HttpResponse getUsers(const HttpRequest& req);
    HttpResponse getUser(const HttpRequest& req);
    HttpResponse createUser(const HttpRequest& req);
    HttpResponse updateUser(const HttpRequest& req);
    HttpResponse deleteUser(const HttpRequest& req);

private:
    sqlite3* database = nullptr;
    std::mutex mutex;
};

#endif
