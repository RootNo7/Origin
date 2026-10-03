#include "engine/physics/physics_world.hpp"

#include <algorithm>
#include <cmath>

namespace origin {

PhysicsWorld::PhysicsWorld(PhysicsSettings settings) : settings_(settings) {}

void PhysicsWorld::step(EntityRegistry& entities, const World& world, double dt_seconds) {
    if (dt_seconds <= 0.0) return;

    for (auto& entity : entities.all()) {
        if (!entity.alive || !entity.body.dynamic || entity.body.mass_kg <= 0.0) {
            continue;
        }

        const double inv_mass = 1.0 / entity.body.mass_kg;
        const Vec2 acceleration = settings_.gravity_m_s2 + entity.body.force * inv_mass;

        entity.body.velocity += acceleration * dt_seconds;
        entity.body.velocity = entity.body.velocity * (1.0 - settings_.linear_damping);
        entity.body.position += entity.body.velocity * dt_seconds;
        entity.body.force = {};

        if (!world.inside(entity.body.position.x, entity.body.position.y)) {
            entity.body.position.x = std::clamp(entity.body.position.x, 0.0, static_cast<double>(world.width()) - 1e-4);
            if (entity.body.position.y < 0.0) {
                entity.body.position.y = 0.0;
                entity.body.velocity.y = -entity.body.velocity.y * entity.body.restitution;
                entity.body.velocity.x *= (1.0 - entity.body.friction);
            }
        }

        if (settings_.collisions_enabled) {
            const double ground = world.ground_height(entity.body.position.x);
            const double floor = ground + entity.body.radius_m;
            if (entity.body.position.y < floor) {
                entity.body.position.y = floor;

                if (std::abs(entity.body.velocity.y) > 0.05) {
                    entity.body.velocity.y = -entity.body.velocity.y * entity.body.restitution;
                } else {
                    entity.body.velocity.y = 0.0;
                }

                entity.body.velocity.x *= (1.0 - entity.body.friction * 0.02);
            }
        }
    }
}

}
