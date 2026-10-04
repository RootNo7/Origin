#pragma once
#include "engine/core/types.hpp"
#include <cstdint>

namespace origin {

enum class AgentActionType : std::uint8_t {
    Observe = 0,
    Move,
    Interact,
    UseItem,
    Craft,
    Communicate
};

struct AgentAction {
    AgentActionType type = AgentActionType::Observe;
    Vec3 direction{};
    EntityId target = 0;
};

struct AgentActionResult {
    bool accepted = false;
    bool changed_world = false;
    std::uint64_t tick = 0;
};

// The action contract is intentionally small. The world remains authoritative;
// this file defines a future integration seam, not developer privileges.
}
