#pragma once
#include "engine/agents/agent_action.hpp"
#include "engine/time/simulation_clock.hpp"
#include "engine/world/world.hpp"
#include "engine/entities/entity_registry.hpp"
#include "engine/physics/physics_world.hpp"
#include "engine/chemistry/chemistry_world.hpp"
#include "engine/environment/environment.hpp"
#include "engine/events/event_bus.hpp"

namespace origin {
class Simulation {
    SimulationClock clock_;
    World world_;
    EntityRegistry entities_;
    PhysicsWorld physics_;
    ChemistryWorld chemistry_;
    Environment environment_;
    EventBus events_;
public:
    Simulation(std::size_t width, std::size_t depth) : world_(width, depth) {}
    void initialize(std::uint32_t seed);
    void step();
    AgentActionResult apply_action(const AgentAction& action);
    SimulationClock& clock() { return clock_; }
    const SimulationClock& clock() const { return clock_; }
    World& world() { return world_; }
    const World& world() const { return world_; }
    EntityRegistry& entities() { return entities_; }
    const EntityRegistry& entities() const { return entities_; }
    Environment& environment() { return environment_; }
    const Environment& environment() const { return environment_; }
    EventBus& events() { return events_; }
};
}
