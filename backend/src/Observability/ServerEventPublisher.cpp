# include "Observability/ServerEventPublisher.h"

void ServerEventPublisher::subscribe(EventObserver* observer) {
    if (observer == nullptr) return;
    observers.push_back(observer);
}

void ServerEventPublisher::publish(const ServerEvent& event) {
    for (EventObserver* observer: observers)
        observer->onEvent(event);
}