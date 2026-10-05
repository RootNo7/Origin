#include "observer/bridge/state_bridge.hpp"
#include "engine/time/simulation_clock.hpp"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>
namespace origin {
namespace {
bool publish_file(const std::filesystem::path& temp, const std::filesystem::path& target) {
    std::error_code ec;
    std::filesystem::rename(temp, target, ec);
    if (!ec) return true;
#ifdef _WIN32
    const auto backup = target.string() + ".bak";
    std::filesystem::remove(backup, ec);
    ec.clear();
    if (!std::filesystem::exists(target) || std::filesystem::rename(target, backup, ec)) return false;
    ec.clear();
    if (!std::filesystem::rename(temp, target, ec)) {
        std::filesystem::remove(target, ec);
        ec.clear();
        std::filesystem::rename(backup, target, ec);
        return false;
    }
    std::filesystem::remove(backup, ec);
#endif
    return !ec;
}
bool write_terrain_if_changed(const Simulation& simulation, const std::filesystem::path& state_path) {
    const auto terrain_path = state_path.parent_path() / "terrain.txt";
    std::uint64_t existing_revision = std::numeric_limits<std::uint64_t>::max();
    std::ifstream existing(terrain_path);
    std::string label;
    int terrain_version = 0;
    if (existing >> label >> terrain_version && label == "ORIGIN_TERRAIN" && terrain_version == 1) {
        std::string revision_label;
        if (existing >> revision_label >> existing_revision && revision_label == "revision" && existing_revision == simulation.world().terrain_revision()) return true;
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
    return publish_file(temp, terrain_path);
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
    out << "ORIGIN_STATE 4\n";
    out << "time " << simulation.clock().seconds() << "\n";
    out << "tick " << simulation.clock().tick() << "\n";
    out << "width " << world.width() << "\n";
    out << "depth " << world.depth() << "\n";
    out << "sea_level " << world.sea_level_m() << "\n";
    out << "world_revision " << world.revision() << "\n";
    out << "terrain_revision " << world.terrain_revision() << "\n";
    out << "temperature " << simulation.environment().global_temperature_k() << "\n";
    out << "sunlight " << simulation.environment().sunlight() << "\n";
    out << "test_actor " << simulation.human_test_actor_id() << "\n";
    out << "calendar " << calendar.year << ' ' << calendar.month << ' ' << calendar.day_of_month << ' ' << calendar.day_of_week << ' ' << calendar.hour << ' ' << calendar.minute << ' ' << calendar.second << '\n';
    if (const auto* actor = simulation.entities().find(simulation.human_test_actor_id()))
        out << "test_inventory " << actor->inventory.amount_of(ResourceKind::Stone) << ' ' << actor->inventory.amount_of(ResourceKind::Wood) << ' ' << actor->inventory.amount_of(ResourceKind::Water) << ' ' << actor->inventory.amount_of(ResourceKind::Soil) << '\n';
    else out << "test_inventory 0 0 0 0\n";
    out << "terrain_file terrain.txt\n";
    out << "resources " << world.resources().size() << '\n';
    for (const auto& resource : world.resources()) out << resource.id << ' ' << static_cast<int>(resource.kind) << ' ' << resource.position.x << ' ' << resource.position.y << ' ' << resource.position.z << ' ' << resource.remaining << ' ' << resource.max_amount << '\n';
    out << "entities " << simulation.entities().size() << '\n';
    for (const auto& entity : simulation.entities().all()) out << entity.id << ' ' << entity.body.position.x << ' ' << entity.body.position.y << ' ' << entity.body.position.z << ' ' << entity.body.radius_m << ' ' << entity.alive << ' ' << entity.body.dynamic << ' ' << entity.name << '\n';
    out << "END\n";
    out.flush();
    if (!out.good()) return false;
    out.close();
    return publish_file(temp, path);
}
}
