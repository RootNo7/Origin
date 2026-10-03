#include "storage/serialization/world_serializer.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>

namespace origin {
namespace {

constexpr const char* MAGIC = "ORIGIN_STATE 1";

int material_to_int(Material material) { return static_cast<int>(material); }
Material material_from_int(int value) {
    if (value < 0 || value > static_cast<int>(Material::Ice)) return Material::Rock;
    return static_cast<Material>(value);
}

}

bool WorldSerializer::save(const Simulation& simulation, const std::filesystem::path& path, std::string& error) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        error = "could not open save file for writing: " + path.string();
        return false;
    }

    out << MAGIC << '\n';
    out << std::setprecision(17);
    out << "CLOCK " << simulation.clock().tick() << ' ' << simulation.clock().seconds()
        << ' ' << simulation.clock().fixed_dt() << ' ' << simulation.clock().speed()
        << ' ' << simulation.clock().paused() << '\n';
    out << "WORLD " << simulation.world().width() << ' ' << simulation.world().height()
        << ' ' << simulation.world().sea_level_m() << '\n';
    out << "COLUMNS " << simulation.world().columns().size() << '\n';

    for (const auto& column : simulation.world().columns()) {
        out << column.ground_height_m << ' ' << column.temperature_k << ' '
            << column.water_depth_m << ' ' << column.humidity << '\n';
    }

    out << "ENTITIES " << simulation.entities().all().size() << '\n';
    for (const auto& entity : simulation.entities().all()) {
        out << entity.id << ' ' << std::quoted(entity.name) << ' '
            << material_to_int(entity.material) << ' ' << entity.alive << ' '
            << entity.body.position.x << ' ' << entity.body.position.y << ' '
            << entity.body.velocity.x << ' ' << entity.body.velocity.y << ' '
            << entity.body.force.x << ' ' << entity.body.force.y << ' '
            << entity.body.mass_kg << ' ' << entity.body.radius_m << ' '
            << entity.body.dynamic << ' ' << entity.body.restitution << ' '
            << entity.body.friction << '\n';
    }

    if (!out.good()) {
        error = "failed while writing save file: " + path.string();
        return false;
    }
    return true;
}

bool WorldSerializer::load(Simulation& simulation, const std::filesystem::path& path, std::string& error) {
    std::ifstream in(path);
    if (!in) {
        error = "could not open save file: " + path.string();
        return false;
    }

    std::string line;
    if (!std::getline(in, line) || line != MAGIC) {
        error = "invalid Origin save header";
        return false;
    }

    // Save/load is intentionally versioned and text-based for now so the
    // format stays inspectable during early development.
    if (!std::getline(in, line) || line.rfind("CLOCK ", 0) != 0) {
        error = "missing CLOCK record";
        return false;
    }
    std::stringstream clock_stream(line.substr(6));
    std::uint64_t tick{};
    double seconds{}, dt{}, speed{};
    bool paused{};
    if (!(clock_stream >> tick >> seconds >> dt >> speed >> paused)) {
        error = "invalid CLOCK record";
        return false;
    }

    if (!std::getline(in, line) || line.rfind("WORLD ", 0) != 0) {
        error = "missing WORLD record";
        return false;
    }
    std::stringstream world_stream(line.substr(6));
    std::size_t width{}, height{};
    double sea_level{};
    if (!(world_stream >> width >> height >> sea_level) || width == 0 || height == 0) {
        error = "invalid WORLD record";
        return false;
    }

    if (!std::getline(in, line) || line.rfind("COLUMNS ", 0) != 0) {
        error = "missing COLUMNS record";
        return false;
    }
    std::size_t column_count{};
    if (!(std::stringstream(line.substr(8)) >> column_count) || column_count != width) {
        error = "invalid COLUMNS record";
        return false;
    }

    Simulation loaded(width, height);
    for (std::size_t i = 0; i < column_count; ++i) {
        if (!std::getline(in, line)) {
            error = "unexpected end of file while reading columns";
            return false;
        }
        std::stringstream ss(line);
        auto& column = loaded.world().columns()[i];
        if (!(ss >> column.ground_height_m >> column.temperature_k >> column.water_depth_m >> column.humidity)) {
            error = "invalid column record";
            return false;
        }
    }

    if (!std::getline(in, line) || line.rfind("ENTITIES ", 0) != 0) {
        error = "missing ENTITIES record";
        return false;
    }
    std::size_t entity_count{};
    if (!(std::stringstream(line.substr(9)) >> entity_count)) {
        error = "invalid ENTITIES record";
        return false;
    }

    for (std::size_t i = 0; i < entity_count; ++i) {
        if (!std::getline(in, line)) {
            error = "unexpected end of file while reading entities";
            return false;
        }
        std::stringstream ss(line);
        EntityId id{};
        std::string quoted_name;
        int material_value{};
        int alive_int{}, dynamic_int{};
        Entity entity;

        if (!(ss >> id >> std::quoted(quoted_name) >> material_value >> alive_int
              >> entity.body.position.x >> entity.body.position.y
              >> entity.body.velocity.x >> entity.body.velocity.y
              >> entity.body.force.x >> entity.body.force.y
              >> entity.body.mass_kg >> entity.body.radius_m
              >> dynamic_int >> entity.body.restitution >> entity.body.friction)) {
            error = "invalid entity record";
            return false;
        }

        entity.id = id;
        entity.name = quoted_name;
        entity.material = material_from_int(material_value);
        entity.alive = alive_int != 0;
        entity.body.dynamic = dynamic_int != 0;
        if (!loaded.entities().insert_restored(entity)) {
            error = "duplicate or invalid entity id";
            return false;
        }
    }

    loaded.clock().restore(tick, seconds, dt, speed, paused);

    simulation = std::move(loaded);
    return true;
}

}
