#include "observer/rendering/console_renderer.hpp"

#include <algorithm>
#include <cmath>
#include <string>

namespace origin {
namespace {

char tile_char(const World& world, std::size_t x, std::size_t y) {
    const double ground = world.columns()[x].ground_height_m;
    const double yy = static_cast<double>(y);

    if (yy < ground) {
        if (ground <= world.sea_level_m()) return '~';
        return '#';
    }
    return ' ';
}

}

void ConsoleRenderer::render(const Simulation& simulation, std::ostream& out) {
    const auto& world = simulation.world();
    std::vector<std::string> rows(world.height(), std::string(world.width(), ' '));

    for (std::size_t y = 0; y < world.height(); ++y) {
        for (std::size_t x = 0; x < world.width(); ++x) {
            rows[world.height() - 1 - y][x] = tile_char(world, x, y);
        }
    }

    for (const auto& entity : simulation.entities().all()) {
        if (!entity.alive || !world.inside(entity.body.position.x, entity.body.position.y)) continue;
        const auto x = static_cast<std::size_t>(std::floor(entity.body.position.x));
        const auto y = static_cast<std::size_t>(std::floor(entity.body.position.y));
        const auto row = world.height() - 1 - std::min(y, world.height() - 1);
        if (x < world.width() && row < rows.size()) {
            rows[row][x] = 'O';
        }
    }

    out << "\nOrigin / VEarth\n";
    out << "time=" << simulation.clock().seconds()
        << "s  tick=" << simulation.clock().tick()
        << "  speed=" << simulation.clock().speed()
        << "x  sunlight=" << simulation.environment().sunlight() << '\n';
    out << "+" << std::string(world.width(), '-') << "+\n";
    for (const auto& row : rows) {
        out << '|' << row << "|\n";
    }
    out << "+" << std::string(world.width(), '-') << "+\n";
    out << "# rock/soil  ~ water  O physical entity\n";

    for (const auto& entity : simulation.entities().all()) {
        out << entity.name << "  pos=("
            << entity.body.position.x << ", " << entity.body.position.y
            << ")  vel=(" << entity.body.velocity.x << ", " << entity.body.velocity.y << ")\n";
    }
}

}
