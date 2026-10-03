#include "engine/events/event_bus.hpp"

namespace origin {

void EventBus::subscribe(Handler handler) {
    handlers_.push_back(std::move(handler));
}

void EventBus::publish(const Event& event) const {
    for (const auto& handler : handlers_) {
        handler(event);
    }
}

}
