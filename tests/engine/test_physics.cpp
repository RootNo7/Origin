#include "engine/entities/entity_registry.hpp"
#include "engine/physics/physics_world.hpp"
#include "engine/world/world.hpp"
#include <cassert>

void test_physics_fall() {
    origin::World world(40, 20);
    world.generate_vearth(7);
    origin::EntityRegistry registry;

    origin::RigidBody body;
    body.position = {10.0, 18.0};
    body.mass_kg = 1.0;
    const auto id = registry.create("test", origin::Material::Rock, body);

    origin::PhysicsWorld physics;
    physics.step(registry, world, 1.0 / 60.0);

    const auto* entity = registry.find(id);
    assert(entity != nullptr);
    assert(entity->body.position.y < 18.0);
}
