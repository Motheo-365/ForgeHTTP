#ifndef EVENTOBSERVER_H
#define EVENTOBSERVER_H

// Interface implemented by anything that wants to react to server events.
class EventObserver {
    public:
        void onEvent(const ServerEvent& e) = 0;
};

#endif