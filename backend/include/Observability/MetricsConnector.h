#ifndef METRICSCONNECTOR_H
#define METRICSCONNECTOR_H

#include "HTTP/HttpResponse.h"
#include "ServerEvent.h"
#include "EventObserver.h"
#include <mutex>

// An EventObserver that aggregates counts and timings, exposed via /metrics.
class MetricsConnector : public EventObserver {
    public:
        explicit MetricsConnector(int workers = 0);

        void onEvent(const ServerEvent& event) override;
        HttpResponse getMetrics();

    private:
        int requestCount = 0;
        double averageResponse = 0.0;
        int errorCount = 0;
        int activeConnections = 0;
        int workers = 0;
        mutable std::mutex mutex;
};

#endif
