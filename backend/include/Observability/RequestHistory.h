#ifndef REQUEST_HISTORY_H
#define REQUEST_HISTORY_H

#include "EventObserver.h"
#include "HTTP/HttpResponse.h"

#include <deque>
#include <mutex>

class RequestHistory : public EventObserver {

public:
    explicit RequestHistory(std::size_t maxEntries = 100);
    void onEvent(const ServerEvent& event) override;
    HttpResponse getRequests(std::size_t limit = 100);
    void setMaxEntries(std::size_t maxEntries);

private:
    std::deque<ServerEvent> requests;
    std::size_t maxEntries;
    mutable std::mutex mutex;
};

#endif