#include "engine/simulation/simulation.hpp"
#include "observer/rendering/console_renderer.hpp"
#include "observer/bridge/state_bridge.hpp"
#include "observer/bridge/action_bridge.hpp"
#include "observer/bridge/developer_bridge.hpp"
#include "storage/serialization/world_serializer.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

int main(int argc, char** argv) {
    using namespace origin;
    Simulation simulation(96, 96);
    std::filesystem::create_directories("observer/bridge");
    std::filesystem::create_directories("storage/saves");

    bool bridge_mode = false;
    bool resume = false;
    std::filesystem::path load_path;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--bridge") bridge_mode = true;
        else if (arg == "--resume") resume = true;
        else if (arg == "--load" && i + 1 < argc) load_path = argv[++i];
    }

    bool loaded = false;
    if (!load_path.empty()) loaded = WorldSerializer::load(simulation, load_path);
    else if (resume && std::filesystem::exists("storage/saves/vearth.origin"))
        loaded = WorldSerializer::load(simulation, "storage/saves/vearth.origin");

    if (!loaded) simulation.initialize(7);

    if (bridge_mode) {
        std::cout << (loaded ? "Origin bridge resumed from save.\n" : "Origin bridge started new world.\n");
        const auto command_path = std::filesystem::path("observer/bridge/commands.txt");
        const auto result_path = std::filesystem::path("observer/bridge/results.txt");
        const auto test_command_path = std::filesystem::path("observer/bridge/test_commands.txt");
        const auto test_result_path = std::filesystem::path("observer/bridge/test_results.txt");
        if (!std::filesystem::exists(command_path)) std::ofstream(command_path).close();
        if (!std::filesystem::exists(test_command_path)) std::ofstream(test_command_path).close();
        for (;;) {
            DeveloperBridge::process(simulation, test_command_path, test_result_path);
            ActionBridge::process(simulation, command_path, result_path);
            simulation.step();
            if (!StateBridge::write(simulation, "observer/bridge/state.txt")) return 1;
            if (simulation.clock().tick() % 300 == 0 && !WorldSerializer::save(simulation, "storage/saves/vearth.origin")) return 1;
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
    }

    for (int i = 0; i < 120; ++i) simulation.step();
    ConsoleRenderer::render(simulation, std::cout);
    return WorldSerializer::save(simulation, "storage/saves/vearth.origin") ? 0 : 1;
}
