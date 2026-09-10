#include "Observability/MetricsConnector.h"

#include <sstream>

namespace {

bool isDashboardRequest(const ServerEvent &event) {
    return event.method == "OPTIONS" ||
           event.path == "/health" ||
           event.path == "/metrics" ||
           event.path == "/api/requests";
}

}

MetricsConnector::MetricsConnector(int workers) : workers(workers) {}

void MetricsConnector::onEvent(const ServerEvent& event) {
    if (isDashboardRequest(event)) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);

    if (event.type == ServerEventType::RequestReceived) {
        activeConnections++;
        return;
    }

    if (event.type != ServerEventType::ResponseSent) {
        return;
    }

    requestCount++;
    activeConnections--;

    averageResponse =
        ((averageResponse * (requestCount - 1)) + event.durationMs) /
        requestCount;

    if (event.statusCode >= 400) {
        errorCount++;
    }
}

HttpResponse MetricsConnector::getMetrics() {
    std::lock_guard<std::mutex> lock(mutex);
    std::ostringstream json;

    json << "{"
         << "\"requests\": " << requestCount << ","
         << "\"active_connections\": " << activeConnections << ","
         << "\"average_response_time_ms\": " << averageResponse << ","
         << "\"errors\": " << errorCount << ","
         << "\"workers\": " << workers
         << "}";

    return HttpResponse::json(json.str());
}
