#pragma once
#include "engine/core/types.hpp"
#include "engine/world/world.hpp"
#include <string>

namespace origin {
enum class AgentActionType { GatherResource };

struct AgentAction {
    AgentActionType type = AgentActionType::GatherResource;
    EntityId actor_id = 0;
    EntityId target_id = 0;
    double amount = 1.0;
};

struct AgentActionResult {
    bool accepted = false;
    double amount = 0.0;
    std::string reason;
};

inline const char* resource_kind_name(ResourceKind kind) {
    switch (kind) {
        case ResourceKind::Stone: return "stone";
        case ResourceKind::Wood: return "wood";
        case ResourceKind::Water: return "water";
        case ResourceKind::Soil: return "soil";
    }
    return "unknown";
}
}
