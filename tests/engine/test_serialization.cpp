#include "engine/simulation/simulation.hpp"
#include "storage/serialization/world_serializer.hpp"
#include <cassert>
#include <filesystem>

void test_serialization() {
    origin::Simulation simulation(20, 12);
    simulation.initialize(3);
    simulation.step();

    const auto path = std::filesystem::temp_directory_path() / "origin_test_state.origin";
    std::string error;
    assert(origin::WorldSerializer::save(simulation, path, error));

    origin::Simulation restored(1, 1);
    assert(origin::WorldSerializer::load(restored, path, error));
    assert(restored.world().width() == simulation.world().width());
    assert(restored.world().height() == simulation.world().height());
    assert(restored.entities().size() == simulation.entities().size());
    assert(restored.clock().tick() == simulation.clock().tick());
    assert(origin::nearly_equal(restored.clock().seconds(), simulation.clock().seconds()));

    std::filesystem::remove(path);
}
