#include "Observability/MetricsCollector.h"

#include <sstream>

void MetricsController::onEvent(const ServerEvent& e) {
    if (e.type != ServerEventType::ResponseSent) {
        return;
    }

    requestCount++;

    avgResponse = ((avgResponse * (requestCount - 1)) + e.durationMs) / requestCount;

    if (e.statusCode >= 400) {
        errorCount++;
    }
}

HttpResponse MetricsController::getMetrics() {
    std::ostringstream json;

    json << "{"
         << "\"requests\": " << requestCount << ","
         << "\"active_connections\": 0,"
         << "\"average_response_time_ms\": " << avgResponse << ","
         << "\"errors\": " << errorCount << ","
         << "\"workers\": 0"
         << "}";

    return HttpResponse::json(json.str());
}