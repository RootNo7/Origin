#pragma once

#include "engine/entities/entity_registry.hpp"
#include "engine/world/world.hpp"

namespace origin {

struct PhysicsSettings {
    Vec2 gravity_m_s2{0.0, -9.81};
    double linear_damping{0.002};
    bool collisions_enabled{true};
};

class PhysicsWorld {
public:
    explicit PhysicsWorld(PhysicsSettings settings = {});

    void step(EntityRegistry& entities, const World& world, double dt_seconds);

private:
    PhysicsSettings settings_;
};

}
