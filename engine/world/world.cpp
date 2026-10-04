#include "engine/world/world.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace origin {
namespace { constexpr double kPi = 3.14159265358979323846; }

World::World(std::size_t width, std::size_t depth) : width_(width), depth_(depth), cells_(width * depth) {
    if (width < 2 || depth < 2) throw std::invalid_argument("Origin World requires width/depth >= 2");
}

void World::generate_vearth(std::uint32_t seed) {
    seed_ = seed;
    ++revision_;
    ++terrain_revision_;
    std::mt19937 random(seed);
    std::uniform_real_distribution<double> noise(-1.0, 1.0);

    const double cx = static_cast<double>(width_ - 1) * 0.5;
    const double cz = static_cast<double>(depth_ - 1) * 0.5;
    const double scale = std::max(1.0, static_cast<double>(std::min(width_, depth_)));

    for (std::size_t z = 0; z < depth_; ++z) {
        for (std::size_t x = 0; x < width_; ++x) {
            const double fx = static_cast<double>(x);
            const double fz = static_cast<double>(z);
            const double nx = (fx - cx) / scale;
            const double nz = (fz - cz) / scale;
            const double continent = std::sin(nx * kPi * 2.0) * 1.9
                                   + std::cos(nz * kPi * 1.7) * 1.5
                                   + std::sin((nx + nz) * kPi * 4.0) * 0.7;
            const double detail = std::sin(fx * 0.18) * 0.55
                                + std::cos(fz * 0.16) * 0.45
                                + noise(random) * 0.25;

            auto& c = cell(x, z);
            c.ground_height_m = std::clamp(8.0 + continent + detail, 1.0, 26.0);
            const double latitude = (fz / static_cast<double>(depth_ - 1)) * kPi;
            c.temperature_k = 287.0 - 5.0 * std::cos(latitude) + 1.5 * std::sin(fx * 0.035 + fz * 0.02);
            c.water_depth_m = c.ground_height_m < sea_level_m_ ? sea_level_m_ - c.ground_height_m : 0.0;
            c.humidity = c.water_depth_m > 0.0 ? 0.85 : 0.45;
        }
    }
    generate_resources();
}

void World::generate_resources() {
    resources_.clear();
    next_resource_id_ = 1;

    // Deterministic sparse deposits. They are world state, not decorative scenery.
    for (std::size_t z = 4; z + 4 < depth_; z += 9) {
        for (std::size_t x = 4; x + 4 < width_; x += 11) {
            const auto& c = cell(x, z);
            ResourceDeposit deposit;
            deposit.id = next_resource_id_++;
            deposit.kind = c.water_depth_m > 0.0 ? ResourceKind::Water : (((x + z) % 3 == 0) ? ResourceKind::Wood : ResourceKind::Stone);
            deposit.position = {static_cast<double>(x), c.ground_height_m, static_cast<double>(z)};
            deposit.max_amount = deposit.kind == ResourceKind::Stone ? 25.0 : 12.0;
            deposit.remaining = deposit.max_amount;
            resources_.push_back(deposit);
        }
    }
}

ResourceDeposit* World::find_resource(EntityId id) {
    for (auto& resource : resources_) if (resource.id == id) return &resource;
    return nullptr;
}

const ResourceDeposit* World::find_resource(EntityId id) const {
    for (const auto& resource : resources_) if (resource.id == id) return &resource;
    return nullptr;
}

bool World::gather_resource(EntityId id, double amount, double& gathered) {
    gathered = 0.0;
    if (!std::isfinite(amount) || amount <= 0.0) return false;
    auto* resource = find_resource(id);
    if (!resource || resource->remaining <= 0.0) return false;
    gathered = std::min(amount, resource->remaining);
    resource->remaining -= gathered;
    if (resource->remaining < 1e-9) resource->remaining = 0.0;
    ++revision_;
    return gathered > 0.0;
}

double World::ground_height(double x, double z) const {
    if (cells_.empty()) return 0.0;
    const double max_x = static_cast<double>(width_ - 1);
    const double max_z = static_cast<double>(depth_ - 1);
    const double qx = std::clamp(x, 0.0, max_x);
    const double qz = std::clamp(z, 0.0, max_z);

    const std::size_t x0 = static_cast<std::size_t>(std::floor(qx));
    const std::size_t z0 = static_cast<std::size_t>(std::floor(qz));
    const std::size_t x1 = std::min(x0 + 1, width_ - 1);
    const std::size_t z1 = std::min(z0 + 1, depth_ - 1);
    const double tx = qx - static_cast<double>(x0);
    const double tz = qz - static_cast<double>(z0);

    const double h00 = cell(x0, z0).ground_height_m;
    const double h10 = cell(x1, z0).ground_height_m;
    const double h01 = cell(x0, z1).ground_height_m;
    const double h11 = cell(x1, z1).ground_height_m;
    const double hx0 = h00 * (1.0 - tx) + h10 * tx;
    const double hx1 = h01 * (1.0 - tx) + h11 * tx;
    return hx0 * (1.0 - tz) + hx1 * tz;
}
}
