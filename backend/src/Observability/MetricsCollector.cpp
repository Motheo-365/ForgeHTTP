#include "Observability/MetricsCollector.h"

#include <sstream>

MetricsCollector::MetricsCollector(int workers) : workers(workers) {}

void MetricsCollector::onEvent(const ServerEvent& e) {
    std::lock_guard<std::mutex> lock(mutex);

    if (e.type == ServerEventType::RequestReceived) {
        activeConnections++;
        return;
    }

    if (e.type != ServerEventType::ResponseSent) {
        return;
    }

    requestCount++;
    activeConnections--;

    avgResponse = ((avgResponse * (requestCount - 1)) + e.durationMs) / requestCount;

    if (e.statusCode >= 400) {
        errorCount++;
    }
}

HttpResponse MetricsCollector::getMetrics() {
    std::lock_guard<std::mutex> lock(mutex);
    std::ostringstream json;

    json << "{"
         << "\"requests\": " << requestCount << ","
         << "\"active_connections\": " << activeConnections << ","
         << "\"average_response_time_ms\": " << avgResponse << ","
         << "\"errors\": " << errorCount << ","
         << "\"workers\": " << workers
         << "}";

    return HttpResponse::json(json.str());
}