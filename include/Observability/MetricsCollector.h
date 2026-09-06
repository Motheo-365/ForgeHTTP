#ifndef METRICSCONTROLLER_H
#define METRICSCONTROLLER_H

#include "HttpResponse.h"

// An EventObserver that aggreagtes counts and timings, exposed via /metrics.
class MetricsController {
    public:
        void onEvent(const ServerEvent& e) override;
        HttpResponse getMetrics();

    private:
        int requestCount;
        double avgResponse;
};

#endif