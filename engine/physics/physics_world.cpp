#include "engine/physics/physics_world.hpp"
#include <algorithm>
#include <cmath>

namespace origin {
void PhysicsWorld::step(EntityRegistry& entities, const World& world, double dt) {
    for (auto& entity : entities.all()) {
        if (!entity.alive || !entity.body.dynamic) continue;

        entity.body.velocity.y -= 9.81 * dt;
        entity.body.position += entity.body.velocity * dt;
        entity.body.position.x = std::clamp(entity.body.position.x, 0.0, static_cast<double>(world.width() - 1e-4));
        entity.body.position.z = std::clamp(entity.body.position.z, 0.0, static_cast<double>(world.depth() - 1e-4));

        const double floor = world.ground_height(entity.body.position.x, entity.body.position.z) + entity.body.radius_m;
        if (entity.body.position.y < floor) {
            entity.body.position.y = floor;
            if (std::abs(entity.body.velocity.y) > 0.05)
                entity.body.velocity.y = -entity.body.velocity.y * entity.body.restitution;
            else
                entity.body.velocity.y = 0.0;
            entity.body.velocity.x *= 0.98;
            entity.body.velocity.z *= 0.98;
        }
    }
}
}
