#include "Controllers/HealthController.h"

HttpResponse HealthController::getHealth(const HttpRequest& req) {
    return HttpResponse::json("{\"status\":\"ok\"}");
}