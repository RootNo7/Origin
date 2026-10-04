#pragma once
#include <cstdint>
#include <string>
namespace origin{enum class EventType{SimulationStarted,SimulationStepped};struct Event{std::uint64_t tick;double seconds;EventType type;std::string message;};}
