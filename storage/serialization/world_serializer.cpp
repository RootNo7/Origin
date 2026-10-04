#include "storage/serialization/world_serializer.hpp"
#include <fstream>
#include <iomanip>
#include <limits>
#include <vector>
#include <unordered_set>

namespace origin {
namespace {
template <typename T>
bool read_checked(std::istream& in, T& value) { return static_cast<bool>(in >> value); }
}

bool WorldSerializer::save(const Simulation& simulation, const std::filesystem::path& path) {
    std::error_code ec;
    if (!path.parent_path().empty()) std::filesystem::create_directories(path.parent_path(), ec);
    if (ec) return false;

    const auto temp = path.string() + ".tmp";
    std::ofstream out(temp, std::ios::trunc);
    if (!out) return false;
    out << std::setprecision(17);
    const auto& world = simulation.world();
    const auto& clock = simulation.clock();

    out << "ORIGIN_SAVE 3\n";
    out << "tick " << clock.tick() << "\n";
    out << "seconds " << clock.seconds() << "\n";
    out << "dt " << clock.fixed_dt() << "\n";
    out << "speed " << clock.speed() << "\n";
    out << "paused " << clock.paused() << "\n";
    out << "seed " << world.seed() << "\n";
    out << "width " << world.width() << "\n";
    out << "depth " << world.depth() << "\n";
    out << "sea_level " << world.sea_level_m() << "\n";
    out << "world_revision " << world.revision() << "\n";
    out << "terrain_revision " << world.terrain_revision() << "\n";
    out << "cells " << world.cells().size() << "\n";
    for (std::size_t z = 0; z < world.depth(); ++z) {
        for (std::size_t x = 0; x < world.width(); ++x) {
            const auto& c = world.cell(x, z);
            out << x << ' ' << z << ' ' << c.ground_height_m << ' ' << c.temperature_k << ' '
                << c.water_depth_m << ' ' << c.humidity << '\n';
        }
    }
    out << "resources " << world.resources().size() << ' ' << world.next_resource_id() << "\n";
    for (const auto& r : world.resources()) {
        out << r.id << ' ' << static_cast<int>(r.kind) << ' ' << r.position.x << ' ' << r.position.y << ' '
            << r.position.z << ' ' << r.remaining << ' ' << r.max_amount << '\n';
    }
    out << "entities " << simulation.entities().size() << "\n";
    for (const auto& entity : simulation.entities().all()) {
        out << entity.id << ' ' << static_cast<int>(entity.material) << ' ' << entity.alive << ' '
            << std::quoted(entity.name) << ' '
            << entity.body.position.x << ' ' << entity.body.position.y << ' ' << entity.body.position.z << ' '
            << entity.body.velocity.x << ' ' << entity.body.velocity.y << ' ' << entity.body.velocity.z << ' '
            << entity.body.mass_kg << ' ' << entity.body.radius_m << ' ' << entity.body.restitution << ' '
            << entity.body.dynamic << '\n';
        for (const auto& stack : entity.inventory.stacks)
            out << static_cast<int>(stack.kind) << ' ' << stack.amount << ' ';
        out << '\n';
    }
    out << "environment " << simulation.environment().sunlight() << ' ' << simulation.environment().global_temperature_k() << "\n";
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

bool WorldSerializer::load(Simulation& simulation, const std::filesystem::path& path) {
    std::ifstream in(path);
    if (!in) return false;

    std::string magic;
    int version = 0;
    if (!(in >> magic >> version) || magic != "ORIGIN_SAVE" || version != 3) return false;

    std::uint64_t tick = 0, revision = 0, terrain_revision = 0;
    double seconds = 0.0, dt = 0.0, speed = 1.0;
    bool paused = false;
    std::uint32_t seed = 0;
    std::size_t width = 0, depth = 0, cell_count = 0, resource_count = 0, entity_count = 0;
    double sea_level = 0.0, env_sun = 1.0, env_temp = 288.15;

    std::string label;
    if (!(in >> label) || label != "tick" || !read_checked(in, tick)) return false;
    if (!(in >> label) || label != "seconds" || !read_checked(in, seconds)) return false;
    if (!(in >> label) || label != "dt" || !read_checked(in, dt)) return false;
    if (!(in >> label) || label != "speed" || !read_checked(in, speed)) return false;
    if (!(in >> label) || label != "paused" || !read_checked(in, paused)) return false;
    if (!(in >> label) || label != "seed" || !read_checked(in, seed)) return false;
    if (!(in >> label) || label != "width" || !read_checked(in, width)) return false;
    if (!(in >> label) || label != "depth" || !read_checked(in, depth)) return false;
    if (!(in >> label) || label != "sea_level" || !read_checked(in, sea_level)) return false;
    if (!(in >> label) || label != "world_revision" || !read_checked(in, revision)) return false;
    if (!(in >> label) || label != "terrain_revision" || !read_checked(in, terrain_revision)) return false;
    if (!(in >> label) || label != "cells" || !read_checked(in, cell_count)) return false;
    if (width != simulation.world().width() || depth != simulation.world().depth() || cell_count != width * depth) return false;

    std::vector<TerrainCell> cells(cell_count);
    for (std::size_t i = 0; i < cell_count; ++i) {
        std::size_t x = 0, z = 0;
        if (!read_checked(in, x) || !read_checked(in, z) || x >= width || z >= depth) return false;
        auto& c = cells[z * width + x];
        if (!read_checked(in, c.ground_height_m) || !read_checked(in, c.temperature_k) ||
            !read_checked(in, c.water_depth_m) || !read_checked(in, c.humidity)) return false;
    }

    EntityId next_resource_id = 1;
    if (!(in >> label) || label != "resources" || !read_checked(in, resource_count) || !read_checked(in, next_resource_id)) return false;
    std::vector<ResourceDeposit> resources(resource_count);
    for (auto& r : resources) {
        int kind = 0;
        if (!read_checked(in, r.id) || !read_checked(in, kind) || kind < 0 || kind > 3 ||
            !read_checked(in, r.position.x) || !read_checked(in, r.position.y) || !read_checked(in, r.position.z) ||
            !read_checked(in, r.remaining) || !read_checked(in, r.max_amount)) return false;
        r.kind = static_cast<ResourceKind>(kind);
        if (!r.id || r.remaining < 0.0 || r.max_amount < 0.0 || r.remaining > r.max_amount) return false;
    }

    if (!(in >> label) || label != "entities" || !read_checked(in, entity_count)) return false;
    std::vector<Entity> entities(entity_count);
    for (auto& entity : entities) {
        int material = 0;
        if (!read_checked(in, entity.id) || !read_checked(in, material) || material < 0 || material > 5 ||
            !read_checked(in, entity.alive) || !(in >> std::quoted(entity.name)) ||
            !read_checked(in, entity.body.position.x) || !read_checked(in, entity.body.position.y) || !read_checked(in, entity.body.position.z) ||
            !read_checked(in, entity.body.velocity.x) || !read_checked(in, entity.body.velocity.y) || !read_checked(in, entity.body.velocity.z) ||
            !read_checked(in, entity.body.mass_kg) || !read_checked(in, entity.body.radius_m) ||
            !read_checked(in, entity.body.restitution) || !read_checked(in, entity.body.dynamic)) return false;
        entity.material = static_cast<Material>(material);
        for (auto& stack : entity.inventory.stacks) {
            int kind = 0;
            if (!read_checked(in, kind) || kind < 0 || kind > 3 || !read_checked(in, stack.amount)) return false;
            stack.kind = static_cast<ResourceKind>(kind);
            if (stack.amount < 0.0) return false;
        }
        if (!entity.id || entity.body.mass_kg <= 0.0 || entity.body.radius_m <= 0.0) return false;
    }

    if (!(in >> label) || label != "environment" || !read_checked(in, env_sun) || !read_checked(in, env_temp)) return false;
    if (!(in >> label) || label != "END") return false;

    auto& world = simulation.world();
    std::unordered_set<EntityId> entity_ids;
    for (const auto& entity : entities) if (!entity_ids.insert(entity.id).second) return false;
    std::unordered_set<EntityId> resource_ids;
    for (const auto& resource : resources) if (!resource_ids.insert(resource.id).second) return false;

    world.cells() = std::move(cells);
    world.resources() = std::move(resources);
    world.set_next_resource_id(next_resource_id);
    world.set_revision(revision);
    world.set_terrain_revision(terrain_revision);
    world.set_seed(seed);
    simulation.clock().restore(tick, seconds, dt, speed, paused);
    simulation.environment().restore(env_sun, env_temp);
    simulation.entities().clear();
    for (const auto& entity : entities) if (!simulation.entities().insert_restored(entity)) return false;
    (void)sea_level; // Sea level remains an immutable world rule in this milestone.
    return true;
}
}
