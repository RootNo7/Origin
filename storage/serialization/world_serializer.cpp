#include "storage/serialization/world_serializer.hpp"
#include <fstream>
#include <iomanip>

namespace origin {
bool WorldSerializer::save(const Simulation& simulation, const std::filesystem::path& path) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) return false;

    out << std::setprecision(10);
    const auto& world = simulation.world();
    out << "ORIGIN_STATE 2\n";
    out << "tick " << simulation.clock().tick() << "\n";
    out << "seconds " << simulation.clock().seconds() << "\n";
    out << "width " << world.width() << "\n";
    out << "depth " << world.depth() << "\n";
    out << "sea_level " << world.sea_level_m() << "\n";
    out << "world_revision " << world.revision() << "\n";
    out << "cells\n";
    for (std::size_t z = 0; z < world.depth(); ++z) {
        for (std::size_t x = 0; x < world.width(); ++x) {
            const auto& c = world.cell(x, z);
            out << x << ' ' << z << ' ' << c.ground_height_m << ' ' << c.temperature_k << ' '
                << c.water_depth_m << ' ' << c.humidity << '\n';
        }
    }

    out << "entities " << simulation.entities().size() << "\n";
    for (const auto& entity : simulation.entities().all()) {
        out << entity.id << ' ' << entity.body.position.x << ' ' << entity.body.position.y << ' '
            << entity.body.position.z << ' ' << entity.body.radius_m << ' ' << entity.alive << '\n';
    }
    return out.good();
}
}
