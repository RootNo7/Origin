#include "observer/rendering/console_renderer.hpp"
#include "storage/serialization/world_serializer.hpp"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

int main() {
    using namespace origin;

    Simulation simulation(80, 28);
    simulation.initialize(7);

    const auto save_path = std::filesystem::path("storage") / "saves" / "vearth.origin";
    std::error_code ec;
    std::filesystem::create_directories(save_path.parent_path(), ec);

    simulation.events().subscribe([](const Event& event) {
        if (event.type == EventType::SimulationStarted) {
            std::cout << "[event] " << event.message << '\n';
        }
    });

    ConsoleRenderer::render(simulation, std::cout);

    std::cout << "\nRunning 120 fixed simulation steps...\n";
    for (int i = 0; i < 120; ++i) {
        simulation.step();
        if (i == 29 || i == 59 || i == 89 || i == 119) {
            ConsoleRenderer::render(simulation, std::cout);
        }
    }

    std::string error;
    if (WorldSerializer::save(simulation, save_path, error)) {
        std::cout << "\nSaved: " << save_path.string() << '\n';
    } else {
        std::cerr << "Save failed: " << error << '\n';
        return 1;
    }

    std::cout << "\nOrigin virtual-world run completed.\n";
    return 0;
}
