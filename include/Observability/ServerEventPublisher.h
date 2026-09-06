#ifndef SERVEREVENTPUBLISHER_H
#define SERVEREVENTPUBLISHER_H

#include "EventObserver.h"
#include <vector>

// Maintains the list of subscribers and notifies them when the Server publishes an event.
class ServerEventPublisher : public EventObserver {
    public:
        void subscribe(EventObserver* o);
        void publish(const ServerEvent& e);

    private:
        vector<EventObserver*> observers;
};

#endif