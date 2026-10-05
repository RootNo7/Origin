#include "engine/physics/physics_world.hpp"
#include <algorithm>
#include <cmath>

namespace origin {
void PhysicsWorld::step(EntityRegistry& entities, const World& world, double dt) {
    if (!std::isfinite(dt) || dt <= 0.0) return;
    constexpr double kMaxSubstep = 1.0 / 120.0;
    constexpr int kMaxSubsteps = 480;
    const double raw_substeps = std::ceil(dt / kMaxSubstep);
    const int substeps = std::clamp(std::isfinite(raw_substeps) ? static_cast<int>(std::min(raw_substeps, static_cast<double>(kMaxSubsteps))) : 1, 1, kMaxSubsteps);
    const double sub_dt = dt / static_cast<double>(substeps);

    for (int substep = 0; substep < substeps; ++substep) {
        for (auto& entity : entities.all()) {
            if (!entity.alive || !entity.body.dynamic) continue;

            if (!std::isfinite(entity.body.mass_kg) || entity.body.mass_kg <= 0.0) entity.body.mass_kg = 1.0;
            if (!std::isfinite(entity.body.radius_m) || entity.body.radius_m <= 0.0) entity.body.radius_m = 0.35;
            if (!std::isfinite(entity.body.restitution)) entity.body.restitution = 0.2;
            entity.body.restitution = std::clamp(entity.body.restitution, 0.0, 1.0);

            if (!is_finite(entity.body.position) || !is_finite(entity.body.velocity)) {
                const double x = static_cast<double>(world.width() - 1) * 0.5;
                const double z = static_cast<double>(world.depth() - 1) * 0.5;
                entity.body.position = {x, world.ground_height(x, z) + entity.body.radius_m, z};
                entity.body.velocity = {};
            }

            entity.body.velocity.y -= 9.81 * sub_dt;
            entity.body.position += entity.body.velocity * sub_dt;
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
}
