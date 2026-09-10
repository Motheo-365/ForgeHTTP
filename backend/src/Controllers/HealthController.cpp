#include "Controllers/HealthController.h"

HttpResponse HealthController::getHealth(const HttpRequest&) {
    return HttpResponse::json("{\"status\":\"ok\"}");
}