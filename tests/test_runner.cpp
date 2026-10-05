#include "engine/simulation/simulation.hpp"
#include "engine/time/simulation_clock.hpp"
#include "observer/bridge/state_bridge.hpp"
#include "observer/bridge/action_bridge.hpp"
#include "observer/bridge/developer_bridge.hpp"
#include "storage/serialization/world_serializer.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <sstream>
#include <string>
#include <iostream>

namespace {
std::string read_text(const std::filesystem::path& path) {
    std::ifstream in(path);
    return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

void write_text(const std::filesystem::path& path, const std::string& text) {
    std::ofstream out(path, std::ios::trunc);
    assert(out);
    out << text;
    assert(out.good());
}
}

int main() {
    using namespace origin;
    Simulation simulation(48, 48);
    simulation.initialize(7);

    assert(simulation.world().width() == 48);
    assert(simulation.world().depth() == 48);
    assert(simulation.world().revision() == 1);
    assert(simulation.world().terrain_revision() == 1);
    assert(simulation.entities().size() == 3);
    assert(simulation.human_test_actor_id() != 0);
    assert(simulation.entities().find(simulation.human_test_actor_id())->name == "HumanTester");
    assert(simulation.world().resources().size() > 0);

    const auto before = simulation.entities().all()[0].body.position.y;
    simulation.step();
    const auto after = simulation.entities().all()[0].body.position.y;
    assert(simulation.clock().tick() == 1);
    assert(after < before);
    assert(simulation.world().revision() == 2);
    assert(simulation.world().ground_height(10.0, 10.0) >= 1.0);
    assert(simulation.world().ground_height(10.0, 10.0) <= 26.0);
    assert(std::isfinite(simulation.world().ground_height(std::numeric_limits<double>::quiet_NaN(), 10.0)));
    assert(std::isfinite(simulation.world().ground_height(10.0, std::numeric_limits<double>::infinity())));

    SimulationClock extreme_clock;
    extreme_clock.restore(10, std::numeric_limits<double>::max(), 1e9, std::numeric_limits<double>::max(), false);
    assert(extreme_clock.fixed_dt() <= 0.25);
    assert(extreme_clock.speed() <= 16.0);
    extreme_clock.advance();
    assert(std::isfinite(extreme_clock.seconds()));
    const auto extreme_calendar = calendar_from_seconds(std::numeric_limits<double>::max());
    assert(extreme_calendar.total_seconds == std::numeric_limits<std::uint64_t>::max());

    const auto calendar = calendar_from_seconds((2ULL * 30ULL * 12ULL * 86400ULL) + 3ULL * 86400ULL + 3ULL * 3600ULL + 4ULL * 60ULL + 5ULL);
    assert(calendar.year == 2);
    assert(calendar.month == 0);
    assert(calendar.day_of_month == 3);
    assert(calendar.hour == 3 && calendar.minute == 4 && calendar.second == 5);

    // Authoritative gather using the dedicated human test actor.
    auto& agent_actor = simulation.entities().all()[0];
    const auto resource = simulation.world().resources().front();
    agent_actor.body.position = resource.position;
    AgentAction action{AgentActionType::GatherResource, agent_actor.id, resource.id, 3.0};
    const auto result = simulation.apply_action(action);
    assert(result.accepted);
    assert(result.amount > 0.0 && result.amount <= 3.0);
    assert(agent_actor.inventory.amount_of(resource.kind) > 0.0);
    assert(simulation.world().revision() == 3);

    // Agent actions cannot impersonate the developer-only human test actor.
    const auto protected_result = simulation.apply_action({AgentActionType::GatherResource, simulation.human_test_actor_id(), resource.id, 1.0});
    assert(!protected_result.accepted);
    assert(protected_result.reason == "developer_actor_unavailable");

    // Human/developer channel is isolated from the agent action file.
    const auto test_dir = std::filesystem::temp_directory_path() / "origin_040_test";
    std::filesystem::remove_all(test_dir);
    std::filesystem::create_directories(test_dir);
    const auto action_command = test_dir / "commands.txt";
    const auto action_result = test_dir / "results.txt";
    write_text(action_command, "gather 999999 1 1\n");
    assert(ActionBridge::process(simulation, action_command, action_result) == 1);
    assert(read_text(action_result).find("rejected 0 invalid_actor") != std::string::npos);
    assert(!std::filesystem::exists(action_command.string() + ".processing"));

    write_text(action_command, "unknown 1 2 3\ngather 3\n");
    assert(ActionBridge::process(simulation, action_command, action_result) == 2);
    const auto malformed_output = read_text(action_result);
    assert(malformed_output.find("unsupported_command") != std::string::npos);
    assert(malformed_output.find("malformed_command") != std::string::npos);

    const auto test_command = test_dir / "test_commands.txt";
    const auto test_result = test_dir / "test_results.txt";
    auto& actor = *simulation.entities().find(simulation.human_test_actor_id());
    const auto resource2 = simulation.world().resources()[1];
    std::ostringstream developer_input;
    developer_input << "pose " << actor.id << ' ' << resource2.position.x << ' ' << resource2.position.y << ' ' << resource2.position.z << '\n';
    developer_input << "gather " << actor.id << ' ' << resource2.id << " 2\n";
    write_text(test_command, developer_input.str());
    assert(DeveloperBridge::process(simulation, test_command, test_result) == 2);
    const auto developer_output = read_text(test_result);
    assert(developer_output.find("pose 3 accepted") != std::string::npos);
    assert(developer_output.find("gather 3 ") != std::string::npos);
    assert(actor.inventory.amount_of(resource2.kind) > 0.0);

    const auto bridge_file = test_dir / "state.txt";
    const auto terrain_file = test_dir / "terrain.txt";
    const auto save_file = test_dir / "state.origin";
    assert(StateBridge::write(simulation, bridge_file));
    const auto bridge_text = read_text(bridge_file);
    assert(bridge_text.find("ORIGIN_STATE 4") != std::string::npos);
    assert(bridge_text.find("terrain_file terrain.txt") != std::string::npos);
    assert(bridge_text.find("test_actor 3") != std::string::npos);
    assert(bridge_text.find("test_inventory ") != std::string::npos);
    assert(bridge_text.find("HumanTester") != std::string::npos);
    assert(std::filesystem::exists(terrain_file));

    if (!WorldSerializer::save(simulation, save_file)) {
        std::cerr << "SAVE FAIL next=" << simulation.world().next_resource_id() << " resources=" << simulation.world().resources().size() << " test=" << simulation.human_test_actor_id() << "\n";
        for (const auto &r: simulation.world().resources()) std::cerr << "R "<<r.id<<" "<<r.position.x<<","<<r.position.y<<","<<r.position.z<<" "<<r.remaining<<"/"<<r.max_amount<<"\n";
        for (const auto &e: simulation.entities().all()) std::cerr << "E "<<e.id<<" "<<e.name<<" p="<<e.body.position.x<<","<<e.body.position.y<<","<<e.body.position.z<<" m="<<e.body.mass_kg<<" r="<<e.body.radius_m<<" rest="<<e.body.restitution<<"\n";
        return 89;
    }
    const auto valid_save = read_text(save_file);
    assert(valid_save.find("ORIGIN_SAVE 4") == 0);

    // Downgrade the fixture shape to the 0.3 schema and verify migration adds HumanTester.
    const auto legacy_file = test_dir / "legacy_v3.origin";
    auto legacy = valid_save;
    legacy.replace(legacy.find("ORIGIN_SAVE 4"), std::string("ORIGIN_SAVE 4").size(), "ORIGIN_SAVE 3");
    const auto test_actor_line = legacy.find("test_actor 3\n");
    assert(test_actor_line != std::string::npos);
    legacy.erase(test_actor_line, std::string("test_actor 3\n").size());
    const auto entity_count_marker = legacy.find("entities 3\n");
    assert(entity_count_marker != std::string::npos);
    legacy.replace(entity_count_marker, std::string("entities 3\n").size(), "entities 2\n");
    const auto human_line = legacy.find("HumanTester");
    assert(human_line != std::string::npos);
    const auto human_start = legacy.rfind('\n', human_line);
    const auto human_next = legacy.find('\n', human_line);
    const auto human_inventory_next = legacy.find('\n', human_next + 1);
    assert(human_start != std::string::npos && human_next != std::string::npos && human_inventory_next != std::string::npos);
    legacy.erase(human_start + 1, human_inventory_next - human_start);
    write_text(legacy_file, legacy);
    Simulation migrated(48, 48);
    assert(WorldSerializer::load(migrated, legacy_file));
    assert(migrated.entities().size() == 3);
    assert(migrated.human_test_actor_id() != 0);
    assert(migrated.entities().find(migrated.human_test_actor_id())->name == "HumanTester");

    Simulation restored(48, 48);
    assert(WorldSerializer::load(restored, save_file));
    assert(restored.world().seed() == 7);
    assert(restored.world().revision() == simulation.world().revision());
    assert(restored.world().terrain_revision() == 1);
    assert(restored.entities().size() == 3);
    assert(restored.human_test_actor_id() == simulation.human_test_actor_id());
    assert(restored.entities().find(restored.human_test_actor_id())->inventory.amount_of(resource.kind) > 0.0);
    assert(restored.clock().tick() == simulation.clock().tick());

    // Failed loads must not partially replace the live simulation.
    const auto bad_nan_file = test_dir / "bad_nan.origin";
    auto bad_nan = valid_save;
    const auto first_cell = bad_nan.find("0 0 ");
    assert(first_cell != std::string::npos);
    const auto first_cell_end = bad_nan.find('\n', first_cell);
    bad_nan.replace(first_cell, first_cell_end - first_cell, "0 0 nan 288 0 0.5");
    write_text(bad_nan_file, bad_nan);
    Simulation guard(48, 48);
    guard.initialize(99);
    const auto guard_seed = guard.world().seed();
    assert(!WorldSerializer::load(guard, bad_nan_file));
    assert(guard.world().seed() == guard_seed);
    assert(guard.entities().size() == 3);

    const auto bad_duplicate_file = test_dir / "bad_duplicate.origin";
    auto bad_duplicate = valid_save;
    const auto second_cell = bad_duplicate.find("\n1 0 ");
    assert(second_cell != std::string::npos);
    bad_duplicate.replace(second_cell, 5, "\n0 0 ");
    write_text(bad_duplicate_file, bad_duplicate);
    assert(!WorldSerializer::load(guard, bad_duplicate_file));
    assert(guard.world().seed() == guard_seed);

    const auto bad_schema = test_dir / "bad_schema.origin";
    write_text(bad_schema, "ORIGIN_SAVE 999\n");
    assert(!WorldSerializer::load(restored, bad_schema));

    // Extreme dynamic state must recover to finite values.
    auto& stone = simulation.entities().all()[0];
    stone.body.position.x = std::numeric_limits<double>::quiet_NaN();
    stone.body.position.y = std::numeric_limits<double>::infinity();
    stone.body.radius_m = std::numeric_limits<double>::quiet_NaN();
    stone.body.restitution = std::numeric_limits<double>::quiet_NaN();
    simulation.step();
    assert(is_finite(stone.body.position));
    assert(is_finite(stone.body.velocity));
    assert(std::isfinite(stone.body.radius_m) && stone.body.radius_m > 0.0);

    std::filesystem::remove_all(test_dir);
    return 0;
}
