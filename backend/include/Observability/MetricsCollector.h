#ifndef METRICSCOLLECTOR_H
#define METRICSCOLLECTOR_H

#include "HTTP/HttpResponse.h"
#include "ServerEvent.h"
#include "EventObserver.h"
#include <mutex>

// An EventObserver that aggreagtes counts and timings, exposed via /metrics.
class MetricsCollector : public EventObserver {
    public:
    explicit MetricsCollector(int workers = 0);

        /*
            Increments requestCount,
            updates the running average response time,
            and tracks error counts, based on the event received.
        */
        void onEvent(const ServerEvent& e) override;

        /*
            Builds JDON snapshot of the current counters.
            Registered agains GET /metrics.

            Expected Output:
                {
                    "requests" : 12452,
                    "active_connections" : 17,
                    "average_response_time_ms" : 8.4,
                    "errors" : 31,
                    "workers" : 8
                }
        */
        HttpResponse getMetrics();

    private:
        int requestCount = 0;
        double avgResponse = 0.0;
        int errorCount = 0;
        int activeConnections = 0;
        int workers = 0;
        mutable std::mutex mutex;
};

using MetricsController = MetricsCollector;

#endif