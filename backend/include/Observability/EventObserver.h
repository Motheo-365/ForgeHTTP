#ifndef EVENTOBSERVER_H
#define EVENTOBSERVER_H

# include "ServerEvent.h"

// Interface implemented by anything that wants to react to server events.
class EventObserver {
    public:
        /*
            Called by ServerEventPublisher whenever a subscribed-to event occurs (e.g. RequestReceived, ResponseSent, ServerStarted, ServerStopped)
        */
        virtual void onEvent(const ServerEvent& e) = 0;
};

#endif