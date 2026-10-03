#pragma once

#include "engine/core/types.hpp"

#include <string>

namespace origin {

enum class EventType {
    SimulationStarted,
    SimulationStepped,
    Collision,
    EnvironmentChanged,
    ChemicalStateChanged,
    SaveCompleted,
    LoadCompleted
};

struct Event {
    std::uint64_t tick{};
    double simulation_seconds{};
    EventType type{};
    EntityId source{};
    std::string message;
};

}
