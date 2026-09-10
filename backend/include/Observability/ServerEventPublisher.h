#ifndef SERVEREVENTPUBLISHER_H
#define SERVEREVENTPUBLISHER_H

#include "EventObserver.h"
#include "ServerEvent.h"

#include <vector>

// Maintains the list of subscribers and notifies them when the Server publishes an event.
class ServerEventPublisher {
    public:
        /**
            Adds an observer to the notification list.
            Aggregation - the publisher does not own the observer's lifetime.
        */
        void subscribe(EventObserver* o);

        /**
            Calls onEvent() on every subscribed observer, in subscription order.
        */
        void publish(const ServerEvent& e);

    private:
        std::vector<EventObserver*> observers;
};

#endif