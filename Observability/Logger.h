#ifndef LOGGER_H
#define LOGGER_H

// An EventObserver that writes a human-readable line per event.
class Logger {
    public:
        void onEvent(const ServerEvent& e) override;
};

#endif