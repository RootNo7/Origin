#include "storage/serialization/world_serializer.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>
#include <unordered_set>
#include <vector>

namespace origin {
namespace {
template <typename T>
bool read_checked(std::istream& in, T& value) { return static_cast<bool>(in >> value); }

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

bool finite_nonnegative(double value) { return std::isfinite(value) && value >= 0.0; }
}

bool WorldSerializer::save(const Simulation& simulation, const std::filesystem::path& path) {
    const auto& world = simulation.world();
    const auto& clock = simulation.clock();
    if (world.cells().size() != world.width() * world.depth() || !std::isfinite(world.sea_level_m()) ||
        !std::isfinite(clock.seconds()) || !std::isfinite(clock.fixed_dt()) || !std::isfinite(clock.speed())) return false;

    std::unordered_set<EntityId> entity_ids;
    for (const auto& entity : simulation.entities().all()) {
        if (!entity.id || !entity_ids.insert(entity.id).second || entity.name.size() > 256 || !is_finite(entity.body.position) ||
            !is_finite(entity.body.velocity) || !std::isfinite(entity.body.mass_kg) || entity.body.mass_kg <= 0.0 ||
            !std::isfinite(entity.body.radius_m) || entity.body.radius_m <= 0.0 || !std::isfinite(entity.body.restitution) ||
            entity.body.restitution < 0.0 || entity.body.restitution > 1.0) return false;
        std::unordered_set<int> kinds;
        for (const auto& stack : entity.inventory.stacks) {
            if (!std::isfinite(stack.amount) || stack.amount < 0.0) return false;
            if (stack.amount > 0.0 && !kinds.insert(static_cast<int>(stack.kind)).second) return false;
        }
    }
    const auto* test_actor = simulation.entities().find(simulation.human_test_actor_id());
    if (!test_actor) return false;

    std::unordered_set<EntityId> resource_ids;
    EntityId max_resource_id = 0;
    for (const auto& resource : world.resources()) {
        const int kind = static_cast<int>(resource.kind);
        if (!resource.id || !resource_ids.insert(resource.id).second || kind < 0 || kind > 3 || !is_finite(resource.position) ||
            !finite_nonnegative(resource.remaining) || !finite_nonnegative(resource.max_amount) || resource.remaining > resource.max_amount ||
            resource.position.x < 0.0 || resource.position.z < 0.0 || resource.position.x > static_cast<double>(world.width() - 1) ||
            resource.position.z > static_cast<double>(world.depth() - 1)) return false;
        max_resource_id = std::max(max_resource_id, resource.id);
    }
    if (world.next_resource_id() == 0 || (max_resource_id == std::numeric_limits<EntityId>::max()) || world.next_resource_id() <= max_resource_id) return false;

    std::error_code ec;
    if (!path.parent_path().empty()) std::filesystem::create_directories(path.parent_path(), ec);
    if (ec) return false;

    const auto temp = path.string() + ".tmp";
    std::ofstream out(temp, std::ios::trunc);
    if (!out) return false;
    out << std::setprecision(17);
    out << "ORIGIN_SAVE 4\n";
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
    out << "test_actor " << simulation.human_test_actor_id() << "\n";
    out << "cells " << world.cells().size() << "\n";
    for (std::size_t z = 0; z < world.depth(); ++z)
        for (std::size_t x = 0; x < world.width(); ++x) {
            const auto& c = world.cell(x, z);
            out << x << ' ' << z << ' ' << c.ground_height_m << ' ' << c.temperature_k << ' '
                << c.water_depth_m << ' ' << c.humidity << '\n';
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
    return publish_file(temp, path);
}

bool WorldSerializer::load(Simulation& simulation, const std::filesystem::path& path) {
    std::ifstream in(path);
    if (!in) return false;

    std::string magic;
    int version = 0;
    if (!(in >> magic >> version) || magic != "ORIGIN_SAVE" || (version != 3 && version != 4)) return false;

    std::uint64_t tick = 0, revision = 0, terrain_revision = 0, saved_test_actor_id = 0;
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
    if (version == 4) {
        if (!(in >> label) || label != "test_actor" || !read_checked(in, saved_test_actor_id)) return false;
    }
    if (!(in >> label) || label != "cells" || !read_checked(in, cell_count)) return false;

    const auto& current_world = simulation.world();
    if (width != current_world.width() || depth != current_world.depth() || cell_count == 0 || cell_count != width * depth ||
        !std::isfinite(seconds) || seconds < 0.0 || !std::isfinite(dt) || dt <= 0.0 || !std::isfinite(speed) || speed < 0.0 ||
        !std::isfinite(sea_level) || !nearly_equal(sea_level, current_world.sea_level_m())) return false;

    if (cell_count > 16777216) return false;
    std::vector<TerrainCell> cells(cell_count);
    std::vector<bool> seen_cells(cell_count, false);
    for (std::size_t i = 0; i < cell_count; ++i) {
        std::size_t x = 0, z = 0;
        if (!read_checked(in, x) || !read_checked(in, z) || x >= width || z >= depth) return false;
        const auto idx = z * width + x;
        if (seen_cells[idx]) return false;
        seen_cells[idx] = true;
        auto& c = cells[idx];
        if (!read_checked(in, c.ground_height_m) || !read_checked(in, c.temperature_k) ||
            !read_checked(in, c.water_depth_m) || !read_checked(in, c.humidity) ||
            !std::isfinite(c.ground_height_m) || !std::isfinite(c.temperature_k) || !finite_nonnegative(c.water_depth_m) ||
            !std::isfinite(c.humidity) || c.humidity < 0.0 || c.humidity > 1.0) return false;
    }

    EntityId next_resource_id = 0;
    if (!(in >> label) || label != "resources" || !read_checked(in, resource_count) || !read_checked(in, next_resource_id) || !next_resource_id) return false;
    if (resource_count > 1000000) return false;
    std::vector<ResourceDeposit> resources(resource_count);
    std::unordered_set<EntityId> resource_ids;
    EntityId max_resource_id = 0;
    for (auto& r : resources) {
        int kind = 0;
        if (!read_checked(in, r.id) || !read_checked(in, kind) || kind < 0 || kind > 3 ||
            !read_checked(in, r.position.x) || !read_checked(in, r.position.y) || !read_checked(in, r.position.z) ||
            !read_checked(in, r.remaining) || !read_checked(in, r.max_amount)) return false;
        r.kind = static_cast<ResourceKind>(kind);
        if (!r.id || !resource_ids.insert(r.id).second || !is_finite(r.position) || !finite_nonnegative(r.remaining) ||
            !finite_nonnegative(r.max_amount) || r.remaining > r.max_amount ||
            r.position.x < 0.0 || r.position.z < 0.0 || r.position.x > static_cast<double>(width - 1) ||
            r.position.z > static_cast<double>(depth - 1)) return false;
        max_resource_id = std::max(max_resource_id, r.id);
    }
    if (max_resource_id == std::numeric_limits<EntityId>::max() || next_resource_id <= max_resource_id) return false;

    if (!(in >> label) || label != "entities" || !read_checked(in, entity_count)) return false;
    if (entity_count > 100000) return false;
    std::vector<Entity> entities(entity_count);
    std::unordered_set<EntityId> entity_ids;
    for (auto& entity : entities) {
        int material = 0;
        if (!read_checked(in, entity.id) || !read_checked(in, material) || material < 0 || material > 5 ||
            !read_checked(in, entity.alive) || !(in >> std::quoted(entity.name)) || entity.name.size() > 256 ||
            !read_checked(in, entity.body.position.x) || !read_checked(in, entity.body.position.y) || !read_checked(in, entity.body.position.z) ||
            !read_checked(in, entity.body.velocity.x) || !read_checked(in, entity.body.velocity.y) || !read_checked(in, entity.body.velocity.z) ||
            !read_checked(in, entity.body.mass_kg) || !read_checked(in, entity.body.radius_m) ||
            !read_checked(in, entity.body.restitution) || !read_checked(in, entity.body.dynamic)) return false;
        entity.material = static_cast<Material>(material);
        if (!entity.id || !entity_ids.insert(entity.id).second || !is_finite(entity.body.position) || !is_finite(entity.body.velocity) ||
            !std::isfinite(entity.body.mass_kg) || entity.body.mass_kg <= 0.0 || !std::isfinite(entity.body.radius_m) || entity.body.radius_m <= 0.0 ||
            !std::isfinite(entity.body.restitution) || entity.body.restitution < 0.0 || entity.body.restitution > 1.0) return false;

        std::unordered_set<int> kinds;
        for (auto& stack : entity.inventory.stacks) {
            int kind = 0;
            if (!read_checked(in, kind) || kind < 0 || kind > 3 || !read_checked(in, stack.amount) ||
                !std::isfinite(stack.amount) || stack.amount < 0.0) return false;
            stack.kind = static_cast<ResourceKind>(kind);
            if (stack.amount > 0.0 && !kinds.insert(kind).second) return false;
        }
    }

    if (!(in >> label) || label != "environment" || !read_checked(in, env_sun) || !read_checked(in, env_temp) ||
        !std::isfinite(env_sun) || !std::isfinite(env_temp) || env_sun < 0.0 || env_sun > 1.0) return false;
    if (!(in >> label) || label != "END") return false;
    std::string trailing;
    if (in >> trailing) return false;

    EntityId loaded_test_actor = saved_test_actor_id;
    if (version == 3) {
        loaded_test_actor = 0;
        for (const auto& entity : entities) if (entity.name == "HumanTester") loaded_test_actor = entity.id;
        if (loaded_test_actor == 0) {
            RigidBody tester;
            tester.dynamic = false;
            tester.mass_kg = 70.0;
            tester.radius_m = 0.35;
            const double x = static_cast<double>(width - 1) * 0.5;
            const double z = static_cast<double>(depth - 1) * 0.5;
            tester.position = {x, cells[static_cast<std::size_t>(z) * width + static_cast<std::size_t>(x)].ground_height_m + tester.radius_m, z};
            const EntityId next_entity_id = entity_ids.empty() ? 1 : *std::max_element(entity_ids.begin(), entity_ids.end());
            if (next_entity_id == std::numeric_limits<EntityId>::max()) return false;
            Entity migrated;
            migrated.id = next_entity_id + 1;
            migrated.name = "HumanTester";
            migrated.body = tester;
            migrated.material = Material::Rock;
            migrated.alive = true;
            entities.push_back(migrated);
            loaded_test_actor = migrated.id;
        }
    }

    bool test_actor_present = false;
    for (const auto& entity : entities) if (entity.id == loaded_test_actor && entity.name == "HumanTester") test_actor_present = true;
    if (!test_actor_present || loaded_test_actor == 0) return false;

    auto& world = simulation.world();
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
    simulation.set_human_test_actor_id(loaded_test_actor);
    return true;
}
}
