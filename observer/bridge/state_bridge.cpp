#include "observer/bridge/state_bridge.hpp"
#include "engine/time/simulation_clock.hpp"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>

namespace origin {
namespace {
bool write_terrain_if_changed(const Simulation& simulation, const std::filesystem::path& state_path) {
    const auto terrain_path = state_path.parent_path() / "terrain.txt";
    std::uint64_t existing_revision = std::numeric_limits<std::uint64_t>::max();
    std::ifstream existing(terrain_path);
    std::string label;
    int terrain_version = 0;
    if (existing >> label >> terrain_version && label == "ORIGIN_TERRAIN" && terrain_version == 1) {
        std::string revision_label;
        if (existing >> revision_label >> existing_revision && revision_label == "revision" &&
            existing_revision == simulation.world().terrain_revision()) return true;
    }

    const auto temp = terrain_path.string() + ".tmp";
    std::ofstream out(temp, std::ios::trunc);
    if (!out) return false;
    out << std::setprecision(10);
    const auto& world = simulation.world();
    out << "ORIGIN_TERRAIN 1\n";
    out << "revision " << world.terrain_revision() << "\n";
    out << "width " << world.width() << "\n";
    out << "depth " << world.depth() << "\n";
    out << "sea_level " << world.sea_level_m() << "\n";
    for (std::size_t z = 0; z < world.depth(); ++z)
        for (std::size_t x = 0; x < world.width(); ++x) {
            const auto& c = world.cell(x, z);
            out << x << ' ' << z << ' ' << c.ground_height_m << ' ' << c.water_depth_m << '\n';
        }
    out << "END\n";
    out.flush();
    if (!out.good()) return false;
    out.close();

    std::error_code ec;
    std::filesystem::rename(temp, terrain_path, ec);
    if (ec) {
        std::filesystem::remove(terrain_path, ec);
        ec.clear();
        std::filesystem::rename(temp, terrain_path, ec);
    }
    return !ec;
}
}

bool StateBridge::write(const Simulation& simulation, const std::filesystem::path& path) {
    std::error_code ec;
    if (!path.parent_path().empty()) std::filesystem::create_directories(path.parent_path(), ec);
    if (ec || !write_terrain_if_changed(simulation, path)) return false;

    const auto temp = path.string() + ".tmp";
    std::ofstream out(temp, std::ios::trunc);
    if (!out) return false;
    out << std::setprecision(10);
    const auto& world = simulation.world();
    const auto calendar = calendar_from_seconds(simulation.clock().seconds());
    out << "ORIGIN_STATE 3\n";
    out << "time " << simulation.clock().seconds() << "\n";
    out << "tick " << simulation.clock().tick() << "\n";
    out << "width " << world.width() << "\n";
    out << "depth " << world.depth() << "\n";
    out << "sea_level " << world.sea_level_m() << "\n";
    out << "world_revision " << world.revision() << "\n";
    out << "terrain_revision " << world.terrain_revision() << "\n";
    out << "temperature " << simulation.environment().global_temperature_k() << "\n";
    out << "sunlight " << simulation.environment().sunlight() << "\n";
    out << "calendar " << calendar.year << ' ' << calendar.month << ' ' << calendar.day_of_month << ' '
        << calendar.day_of_week << ' ' << calendar.hour << ' ' << calendar.minute << ' ' << calendar.second << '\n';
    out << "terrain_file terrain.txt\n";
    out << "resources " << world.resources().size() << '\n';
    for (const auto& resource : world.resources()) {
        out << resource.id << ' ' << static_cast<int>(resource.kind) << ' ' << resource.position.x << ' '
            << resource.position.y << ' ' << resource.position.z << ' ' << resource.remaining << ' ' << resource.max_amount << '\n';
    }
    out << "entities " << simulation.entities().size() << '\n';
    for (const auto& entity : simulation.entities().all()) {
        out << entity.id << ' ' << entity.body.position.x << ' ' << entity.body.position.y << ' '
            << entity.body.position.z << ' ' << entity.body.radius_m << '\n';
    }
    out << "END\n";
    out.flush();
    if (!out.good()) return false;
    out.close();

    std::filesystem::rename(temp, path, ec);
    if (ec) {
        std::filesystem::remove(path, ec);
        ec.clear();
        std::filesystem::rename(temp, path, ec);
    }
    return !ec;
}
}
