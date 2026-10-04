#pragma once
#include "engine/events/event.hpp"
#include <functional>
#include <vector>
namespace origin{class EventBus{std::vector<std::function<void(const Event&)>> h_;public:void subscribe(std::function<void(const Event&)>h){h_.push_back(std::move(h));}void publish(const Event&e)const{for(const auto&fn:h_)fn(e);}};}
