#ifndef LOGGER_H
#define LOGGER_H

#include "ServerEvent.h"
#include "EventObserver.h"

// An EventObserver that writes a human-readable line per event.
class Logger : public EventObserver {
    public:
        /*
            Formats and prints the event, e.g. a line noting the server started or stopped.
            (Request.response-level logging is handled by LoggerMiddle; this Logger covers serverlifecycle events.)
        */
        void onEvent(const ServerEvent& e) override;
};

#endif