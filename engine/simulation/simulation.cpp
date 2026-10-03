#include "engine/simulation/simulation.hpp"

namespace origin {

Simulation::Simulation(std::size_t width, std::size_t height)
    : world_(width, height) {}

void Simulation::initialize(std::uint32_t seed) {
    clock_.reset(1.0 / 30.0);
    environment_.reset();
    world_.generate_vearth(seed);
    entities_ = EntityRegistry{};

    // A small set of ordinary physical objects gives the world something
    // to demonstrate immediately without introducing scripted behaviour.
    RigidBody rock_body;
    rock_body.position = {world_.width() * 0.25, world_.height() * 0.75};
    rock_body.velocity = {1.2, 0.0};
    rock_body.mass_kg = 40.0;
    rock_body.radius_m = 0.45;
    rock_body.restitution = 0.35;
    rock_body.friction = 0.75;
    entities_.create("Stone-1", Material::Rock, rock_body);

    RigidBody rock_body_2 = rock_body;
    rock_body_2.position = {world_.width() * 0.52, world_.height() * 0.82};
    rock_body_2.velocity = {-0.8, 0.0};
    entities_.create("Stone-2", Material::Rock, rock_body_2);

    RigidBody sample_body;
    sample_body.position = {world_.width() * 0.78, world_.height() * 0.55};
    sample_body.mass_kg = 5.0;
    sample_body.radius_m = 0.25;
    sample_body.restitution = 0.5;
    sample_body.friction = 0.6;
    entities_.create("Mineral-1", Material::Rock, sample_body);

    events_.publish({clock_.tick(), clock_.seconds(), EventType::SimulationStarted, 0,
                     "VEarth initialized"});
}

void Simulation::step() {
    if (clock_.paused()) return;

    const double dt = clock_.fixed_dt() * clock_.speed();
    const double previous_sunlight = environment_.sunlight();

    environment_.step(world_, clock_.seconds(), dt);
    chemistry_.update(world_);
    physics_.step(entities_, world_, dt);
    clock_.advance();

    if (!nearly_equal(previous_sunlight, environment_.sunlight(), 1e-6)) {
        events_.publish({clock_.tick(), clock_.seconds(), EventType::EnvironmentChanged, 0,
                         "Day/night cycle updated"});
    }

    events_.publish({clock_.tick(), clock_.seconds(), EventType::SimulationStepped, 0,
                     "Simulation step completed"});
}

}
