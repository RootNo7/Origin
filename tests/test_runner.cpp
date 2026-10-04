#include "engine/simulation/simulation.hpp"
#include "observer/bridge/state_bridge.hpp"
#include "storage/serialization/world_serializer.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>

int main() {
    origin::Simulation simulation(48, 48);
    simulation.initialize(7);

    assert(simulation.world().width() == 48);
    assert(simulation.world().depth() == 48);
    assert(simulation.world().revision() == 1);
    assert(simulation.entities().size() == 2);

    const auto before = simulation.entities().all()[0].body.position.y;
    simulation.step();
    const auto after = simulation.entities().all()[0].body.position.y;

    assert(simulation.clock().tick() == 1);
    assert(after < before);
    assert(simulation.world().ground_height(10.0, 10.0) >= 1.0);
    assert(simulation.world().ground_height(10.0, 10.0) <= 26.0);

    const std::filesystem::path test_dir = std::filesystem::temp_directory_path() / "origin_foundation_test";
    std::filesystem::create_directories(test_dir);
    const auto bridge_file = test_dir / "state.txt";
    const auto save_file = test_dir / "state.origin";

    assert(origin::StateBridge::write(simulation, bridge_file));
    assert(origin::WorldSerializer::save(simulation, save_file));

    std::ifstream bridge(bridge_file);
    const std::string bridge_text((std::istreambuf_iterator<char>(bridge)), std::istreambuf_iterator<char>());
    assert(bridge_text.find("ORIGIN_STATE 2") != std::string::npos);
    assert(bridge_text.find("depth 48") != std::string::npos);
    assert(bridge_text.find("world_revision 1") != std::string::npos);
    assert(bridge_text.find("terrain\n") != std::string::npos);
    assert(bridge_text.find("entities 2") != std::string::npos);

    std::filesystem::remove_all(test_dir);
    return 0;
}
