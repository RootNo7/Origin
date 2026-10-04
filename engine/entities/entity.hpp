#pragma once
#include "engine/core/types.hpp"
#include "engine/world/world.hpp"
#include <string>

namespace origin {
struct RigidBody {
    Vec3 position{};
    Vec3 velocity{};
    double mass_kg = 1.0;
    double radius_m = 0.35;
    double restitution = 0.2;
    bool dynamic = true;
};

struct Entity {
    EntityId id = 0;
    std::string name;
    RigidBody body;
    Material material = Material::Rock;
    bool alive = true;
};
}
