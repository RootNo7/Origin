#include "engine/simulation/simulation.hpp"
#include "engine/time/simulation_clock.hpp"
#include "observer/bridge/state_bridge.hpp"
#include "observer/bridge/action_bridge.hpp"
#include "storage/serialization/world_serializer.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>

int main() {
    using namespace origin;
    Simulation simulation(48, 48);
    simulation.initialize(7);

    assert(simulation.world().width() == 48);
    assert(simulation.world().depth() == 48);
    assert(simulation.world().revision() == 1);
    assert(simulation.world().terrain_revision() == 1);
    assert(simulation.entities().size() == 2);
    assert(simulation.world().resources().size() > 0);

    const auto before = simulation.entities().all()[0].body.position.y;
    simulation.step();
    const auto after = simulation.entities().all()[0].body.position.y;
    assert(simulation.clock().tick() == 1);
    assert(after < before);
    assert(simulation.world().ground_height(10.0, 10.0) >= 1.0);
    assert(simulation.world().ground_height(10.0, 10.0) <= 26.0);

    const auto calendar = calendar_from_seconds((2ULL * 30ULL * 12ULL * 86400ULL) + 3ULL * 86400ULL + 3ULL * 3600ULL + 4ULL * 60ULL + 5ULL);
    assert(calendar.year == 2);
    assert(calendar.month == 0);
    assert(calendar.day_of_month == 3);
    assert(calendar.hour == 3 && calendar.minute == 4 && calendar.second == 5);

    // Exercise the authoritative action path using a nearby resource.
    auto& actor = simulation.entities().all()[0];
    const auto resource = simulation.world().resources().front();
    actor.body.position = resource.position;
    AgentAction action{AgentActionType::GatherResource, actor.id, resource.id, 3.0};
    const auto result = simulation.apply_action(action);
    assert(result.accepted);
    assert(result.amount > 0.0 && result.amount <= 3.0);
    assert(actor.inventory.stacks[0].amount > 0.0);
    assert(simulation.world().revision() == 2);

    const std::filesystem::path test_dir = std::filesystem::temp_directory_path() / "origin_030_test";
    std::filesystem::remove_all(test_dir);
    std::filesystem::create_directories(test_dir);
    const auto action_command = test_dir / "commands.txt";
    const auto action_result = test_dir / "results.txt";
    std::ofstream command(action_command);
    command << "gather " << actor.id << ' ' << resource.id << " 2\n";
    command.close();
    assert(ActionBridge::process(simulation, action_command, action_result) == 1);
    std::ifstream action_output(action_result);
    std::string action_text((std::istreambuf_iterator<char>(action_output)), std::istreambuf_iterator<char>());
    assert(action_text.find("accepted") != std::string::npos);

    const auto bridge_file = test_dir / "state.txt";
    const auto terrain_file = test_dir / "terrain.txt";
    const auto save_file = test_dir / "state.origin";

    assert(StateBridge::write(simulation, bridge_file));
    std::ifstream bridge(bridge_file);
    const std::string bridge_text((std::istreambuf_iterator<char>(bridge)), std::istreambuf_iterator<char>());
    assert(bridge_text.find("ORIGIN_STATE 3") != std::string::npos);
    assert(bridge_text.find("terrain_file terrain.txt") != std::string::npos);
    assert(bridge_text.find("terrain_revision 1") != std::string::npos);
    assert(bridge_text.find("calendar ") != std::string::npos);
    assert(std::filesystem::exists(terrain_file));

    assert(WorldSerializer::save(simulation, save_file));
    Simulation restored(48, 48);
    assert(WorldSerializer::load(restored, save_file));
    assert(restored.world().seed() == 7);
    assert(restored.world().revision() == simulation.world().revision());
    assert(restored.world().terrain_revision() == 1);
    assert(restored.entities().size() == 2);
    assert(restored.entities().all()[0].inventory.stacks[0].amount > 0.0);
    assert(restored.clock().tick() == simulation.clock().tick());

    std::ofstream bad(test_dir / "bad.origin");
    bad << "ORIGIN_SAVE 999\n";
    bad.close();
    assert(!WorldSerializer::load(restored, test_dir / "bad.origin"));

    std::filesystem::remove_all(test_dir);
    return 0;
}
