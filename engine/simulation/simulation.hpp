#pragma once

#include "engine/chemistry/chemistry_world.hpp"
#include "engine/entities/entity_registry.hpp"
#include "engine/environment/environment.hpp"
#include "engine/events/event_bus.hpp"
#include "engine/physics/physics_world.hpp"
#include "engine/time/simulation_clock.hpp"
#include "engine/world/world.hpp"

namespace origin {

class Simulation {
public:
    Simulation(std::size_t width, std::size_t height);

    void initialize(std::uint32_t seed = 7);
    void step();

    [[nodiscard]] SimulationClock& clock() { return clock_; }
    [[nodiscard]] const SimulationClock& clock() const { return clock_; }
    [[nodiscard]] World& world() { return world_; }
    [[nodiscard]] const World& world() const { return world_; }
    [[nodiscard]] EntityRegistry& entities() { return entities_; }
    [[nodiscard]] const EntityRegistry& entities() const { return entities_; }
    [[nodiscard]] const Environment& environment() const { return environment_; }
    [[nodiscard]] EventBus& events() { return events_; }

private:
    SimulationClock clock_;
    World world_;
    EntityRegistry entities_;
    PhysicsWorld physics_;
    ChemistryWorld chemistry_;
    Environment environment_;
    EventBus events_;
};

}
