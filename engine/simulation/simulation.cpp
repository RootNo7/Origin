#include "engine/simulation/simulation.hpp"

namespace origin {
void Simulation::initialize(std::uint32_t seed) {
    clock_.reset();
    environment_.reset();
    world_.generate_vearth(seed);

    RigidBody stone;
    stone.mass_kg = 40.0;
    stone.radius_m = 0.45;
    stone.restitution = 0.35;
    stone.position = {20.0, world_.ground_height(20.0, 20.0) + 6.0, 20.0};
    stone.velocity = {1.2, 0.0, 0.4};
    entities_.create("Stone-1", Material::Rock, stone);

    stone.position = {42.0, world_.ground_height(42.0, 34.0) + 7.0, 34.0};
    stone.velocity = {-0.8, 0.0, -0.3};
    entities_.create("Stone-2", Material::Rock, stone);

    events_.publish({0, 0.0, EventType::SimulationStarted, "VEarth 3D heightfield initialized"});
}

void Simulation::step() {
    const double dt = clock_.fixed_dt();
    environment_.step(world_, clock_.seconds(), dt);
    chemistry_.update(world_);
    physics_.step(entities_, world_, dt);
    clock_.advance();
    events_.publish({clock_.tick(), clock_.seconds(), EventType::SimulationStepped, "step"});
}
}
