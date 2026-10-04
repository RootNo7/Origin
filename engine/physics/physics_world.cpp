#include "engine/physics/physics_world.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace origin {
void PhysicsWorld::step(EntityRegistry& entities, const World& world, double dt) {
    if (!std::isfinite(dt) || dt <= 0.0) return;
    // Keep collision integration stable if a caller supplies a large step or simulation speed.
    constexpr double kMaxSubstep = 1.0 / 120.0;
    const int substeps = std::max(1, static_cast<int>(std::ceil(dt / kMaxSubstep)));
    const double sub_dt = dt / static_cast<double>(substeps);

    for (int substep = 0; substep < substeps; ++substep) {
        for (auto& entity : entities.all()) {
            if (!entity.alive || !entity.body.dynamic) continue;
            if (!std::isfinite(entity.body.position.x) || !std::isfinite(entity.body.position.y) || !std::isfinite(entity.body.position.z) ||
                !std::isfinite(entity.body.velocity.x) || !std::isfinite(entity.body.velocity.y) || !std::isfinite(entity.body.velocity.z)) {
                entity.body.position = {world.width() * 0.5, world.ground_height(world.width() * 0.5, world.depth() * 0.5) + entity.body.radius_m, world.depth() * 0.5};
                entity.body.velocity = {};
            }

            entity.body.velocity.y -= 9.81 * sub_dt;
            entity.body.position += entity.body.velocity * sub_dt;
            entity.body.position.x = std::clamp(entity.body.position.x, 0.0, static_cast<double>(world.width() - 1e-4));
            entity.body.position.z = std::clamp(entity.body.position.z, 0.0, static_cast<double>(world.depth() - 1e-4));

            const double radius = std::max(0.01, entity.body.radius_m);
            const double floor = world.ground_height(entity.body.position.x, entity.body.position.z) + radius;
            if (entity.body.position.y < floor) {
                entity.body.position.y = floor;
                if (std::abs(entity.body.velocity.y) > 0.05)
                    entity.body.velocity.y = -entity.body.velocity.y * std::clamp(entity.body.restitution, 0.0, 1.0);
                else
                    entity.body.velocity.y = 0.0;
                entity.body.velocity.x *= 0.98;
                entity.body.velocity.z *= 0.98;
            }
        }
    }
}
}
