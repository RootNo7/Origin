#include "engine/simulation/simulation.hpp"
#include <cmath>

namespace origin {
void Simulation::initialize(std::uint32_t seed) {
    clock_.reset();
    environment_.reset();
    entities_.clear();
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

    events_.publish({0, 0.0, EventType::SimulationStarted, "VEarth 3D world initialized"});
}

void Simulation::step() {
    const double dt = clock_.fixed_dt();
    environment_.step(world_, clock_.seconds(), dt);
    chemistry_.update(world_);
    physics_.step(entities_, world_, dt);
    clock_.advance();
    events_.publish({clock_.tick(), clock_.seconds(), EventType::SimulationStepped, "step"});
}

AgentActionResult Simulation::apply_action(const AgentAction& action) {
    AgentActionResult result;
    if (action.type != AgentActionType::GatherResource) {
        result.reason = "unsupported_action";
        return result;
    }
    Entity* actor = entities_.find(action.actor_id);
    if (!actor || !actor->alive) { result.reason = "invalid_actor"; return result; }
    if (!std::isfinite(action.amount) || action.amount <= 0.0) { result.reason = "invalid_amount"; return result; }

    ResourceDeposit* target = world_.find_resource(action.target_id);
    if (!target || target->remaining <= 0.0) { result.reason = "invalid_or_depleted_resource"; return result; }

    const Vec3 d{actor->body.position.x - target->position.x, actor->body.position.y - target->position.y,
                 actor->body.position.z - target->position.z};
    constexpr double kGatherRange = 2.25;
    if ((d.x*d.x + d.y*d.y + d.z*d.z) > kGatherRange * kGatherRange) {
        result.reason = "out_of_range";
        return result;
    }

    if (!actor->inventory.can_add(target->kind)) { result.reason = "inventory_full"; return result; }

    double gathered = 0.0;
    if (!world_.gather_resource(target->id, action.amount, gathered)) {
        result.reason = "gather_failed";
        return result;
    }
    if (!actor->inventory.add(target->kind, gathered)) {
        // Defensive fallback; can_add() should make this unreachable.
        target->remaining += gathered;
        world_.mark_revision();
        result.reason = "inventory_full";
        return result;
    }

    result.accepted = true;
    result.amount = gathered;
    result.reason = resource_kind_name(target->kind);
    return result;
}
}
