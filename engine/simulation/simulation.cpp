#include "engine/simulation/simulation.hpp"
#include <algorithm>
#include <cmath>

namespace origin {
namespace { constexpr double kGatherRange = 2.25; }

void Simulation::initialize(std::uint32_t seed) {
    clock_.reset();
    environment_.reset();
    entities_.clear();
    human_test_actor_id_ = 0;
    world_.generate_vearth(seed);

    RigidBody stone;
    stone.mass_kg = 40.0;
    stone.radius_m = 0.45;
    stone.restitution = 0.35;
    stone.position = {20.0, world_.ground_height(20.0, 20.0) + 6.0, 20.0};
    stone.velocity = {1.2, 0.0, 0.4};
    if (!entities_.create("Stone-1", Material::Rock, stone)) return;

    stone.position = {42.0, world_.ground_height(42.0, 34.0) + 7.0, 34.0};
    stone.velocity = {-0.8, 0.0, -0.3};
    if (!entities_.create("Stone-2", Material::Rock, stone)) return;

    RigidBody tester;
    tester.mass_kg = 70.0;
    tester.radius_m = 0.35;
    tester.restitution = 0.0;
    tester.dynamic = false;
    const double center_x = static_cast<double>(world_.width() - 1) * 0.5;
    const double center_z = static_cast<double>(world_.depth() - 1) * 0.5;
    tester.position = {center_x, world_.ground_height(center_x, center_z) + tester.radius_m, center_z};
    human_test_actor_id_ = entities_.create("HumanTester", Material::Rock, tester);

    events_.publish({0, 0.0, EventType::SimulationStarted, "VEarth 3D world initialized"});
}

void Simulation::step() {
    const double dt = clock_.fixed_dt() * clock_.speed();
    if (clock_.paused()) return;
    environment_.step(world_, clock_.seconds(), dt);
    chemistry_.update(world_);
    physics_.step(entities_, world_, dt);
    clock_.advance();
    events_.publish({clock_.tick(), clock_.seconds(), EventType::SimulationStepped, "step"});
}

bool Simulation::set_human_test_actor_pose(const Vec3& requested_position) {
    if (!is_finite(requested_position) || human_test_actor_id_ == 0) return false;
    auto* actor = entities_.find(human_test_actor_id_);
    if (!actor || !actor->alive) return false;
    if (requested_position.x < 0.0 || requested_position.z < 0.0 ||
        requested_position.x > static_cast<double>(world_.width() - 1) ||
        requested_position.z > static_cast<double>(world_.depth() - 1)) return false;

    const double floor = world_.ground_height(requested_position.x, requested_position.z) + actor->body.radius_m;
    actor->body.position = {requested_position.x, std::max(requested_position.y, floor), requested_position.z};
    actor->body.velocity = {};
    return true;
}

bool Simulation::reset_human_test_actor() {
    if (human_test_actor_id_ == 0) return false;
    const double x = static_cast<double>(world_.width() - 1) * 0.5;
    const double z = static_cast<double>(world_.depth() - 1) * 0.5;
    return set_human_test_actor_pose({x, world_.ground_height(x, z) + 0.35, z});
}

AgentActionResult Simulation::apply_action(const AgentAction& action, ActionSource source) {
    AgentActionResult result;
    if (action.type != AgentActionType::GatherResource) {
        result.reason = "unsupported_action";
        return result;
    }
    if (source == ActionSource::Agent && action.actor_id == human_test_actor_id_) {
        result.reason = "developer_actor_unavailable";
        return result;
    }
    Entity* actor = entities_.find(action.actor_id);
    if (!actor || !actor->alive) { result.reason = "invalid_actor"; return result; }
    if (!std::isfinite(action.amount) || action.amount <= 0.0) { result.reason = "invalid_amount"; return result; }

    ResourceDeposit* target = world_.find_resource(action.target_id);
    if (!target || target->remaining <= 0.0) { result.reason = "invalid_or_depleted_resource"; return result; }
    if (!is_finite(target->position) || !std::isfinite(target->remaining) || !std::isfinite(target->max_amount)) {
        result.reason = "invalid_resource_state";
        return result;
    }
    if (!is_finite(actor->body.position)) { result.reason = "invalid_actor_state"; return result; }

    const Vec3 d{actor->body.position.x - target->position.x, actor->body.position.y - target->position.y,
                 actor->body.position.z - target->position.z};
    if (!std::isfinite(length_squared(d)) || length_squared(d) > kGatherRange * kGatherRange) {
        result.reason = "out_of_range";
        return result;
    }

    if (!actor->inventory.can_add(target->kind, std::min(action.amount, target->remaining))) {
        result.reason = "inventory_full";
        return result;
    }

    double gathered = 0.0;
    if (!world_.gather_resource(target->id, action.amount, gathered)) {
        result.reason = "gather_failed";
        return result;
    }
    if (!actor->inventory.add(target->kind, gathered)) {
        // This is a defensive invariant check. can_add() above guarantees this path is unreachable.
        target->remaining = std::min(target->max_amount, target->remaining + gathered);
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
