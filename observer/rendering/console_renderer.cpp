#include "observer/rendering/console_renderer.hpp"
#include <algorithm>
#include <cmath>

namespace origin {
void ConsoleRenderer::render(const Simulation& simulation, std::ostream& out) {
    const auto& world = simulation.world();
    const std::size_t step_x = std::max<std::size_t>(1, world.width() / 80);
    const std::size_t step_z = std::max<std::size_t>(1, world.depth() / 28);

    out << "\nOrigin / VEarth  time=" << simulation.clock().seconds()
        << " tick=" << simulation.clock().tick() << "\n";

    for (std::size_t z = 0; z < world.depth(); z += step_z) {
        for (std::size_t x = 0; x < world.width(); x += step_x) {
            const auto& cell = world.cell(x, z);
            char marker = cell.ground_height_m < world.sea_level_m() ? '~' : (cell.ground_height_m > 13.0 ? '^' : '.');
            for (const auto& entity : simulation.entities().all()) {
                if (static_cast<std::size_t>(std::round(entity.body.position.x)) == x &&
                    static_cast<std::size_t>(std::round(entity.body.position.z)) == z) {
                    marker = 'O';
                    break;
                }
            }
            out << marker;
        }
        out << '\n';
    }
}
}
