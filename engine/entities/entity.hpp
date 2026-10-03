#pragma once

#include "engine/core/types.hpp"
#include "engine/world/world.hpp"

#include <string>

namespace origin {

struct RigidBody {
    Vec2 position{};
    Vec2 velocity{};
    Vec2 force{};
    double mass_kg{1.0};
    double radius_m{0.35};
    bool dynamic{true};
    double restitution{0.2};
    double friction{0.8};
};

struct Entity {
    EntityId id{};
    std::string name;
    RigidBody body;
    Material material{Material::Rock};
    bool alive{true};
};

}
