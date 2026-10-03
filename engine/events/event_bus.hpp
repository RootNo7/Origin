#pragma once

#include "engine/events/event.hpp"

#include <functional>
#include <vector>

namespace origin {

class EventBus {
public:
    using Handler = std::function<void(const Event&)>;

    void subscribe(Handler handler);
    void publish(const Event& event) const;

private:
    std::vector<Handler> handlers_;
};

}
