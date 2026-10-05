#pragma once
#include "engine/core/types.hpp"
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace origin {

enum class Material { Air, Water, Soil, Rock, Ice, Wood };
enum class ResourceKind { Stone, Wood, Water, Soil };

struct TerrainCell {
    double ground_height_m = 0.0;
    double temperature_k = 288.15;
    double water_depth_m = 0.0;
    double humidity = 0.5;
};

struct ResourceDeposit {
    EntityId id = 0;
    ResourceKind kind = ResourceKind::Stone;
    Vec3 position{};
    double remaining = 0.0;
    double max_amount = 0.0;
};

class World {
    static constexpr std::size_t kMaxDimension = 4096;
    std::size_t width_ = 0;
    std::size_t depth_ = 0;
    double sea_level_m_ = 8.0;
    std::uint32_t seed_ = 0;
    std::vector<TerrainCell> cells_;
    std::vector<ResourceDeposit> resources_;
    std::uint64_t revision_ = 0;
    std::uint64_t terrain_revision_ = 0;
    EntityId next_resource_id_ = 1;
    std::size_t index(std::size_t x, std::size_t z) const { return z * width_ + x; }

public:
    World(std::size_t width, std::size_t depth);
    void generate_vearth(std::uint32_t seed);
    void generate_resources();
    double ground_height(double x, double z) const;
    const TerrainCell& cell(std::size_t x, std::size_t z) const { return cells_.at(index(x, z)); }
    TerrainCell& cell(std::size_t x, std::size_t z) { return cells_.at(index(x, z)); }
    ResourceDeposit* find_resource(EntityId id);
    const ResourceDeposit* find_resource(EntityId id) const;
    bool gather_resource(EntityId id, double amount, double& gathered);
    std::size_t width() const { return width_; }
    std::size_t depth() const { return depth_; }
    double sea_level_m() const { return sea_level_m_; }
    std::uint32_t seed() const { return seed_; }
    std::uint64_t revision() const { return revision_; }
    std::uint64_t terrain_revision() const { return terrain_revision_; }
    const std::vector<TerrainCell>& cells() const { return cells_; }
    std::vector<TerrainCell>& cells() { return cells_; }
    const std::vector<ResourceDeposit>& resources() const { return resources_; }
    std::vector<ResourceDeposit>& resources() { return resources_; }
    EntityId next_resource_id() const { return next_resource_id_; }
    void set_next_resource_id(EntityId id) { next_resource_id_ = id; }
    void mark_revision() { if (revision_ != std::numeric_limits<std::uint64_t>::max()) ++revision_; }
    void set_revision(std::uint64_t revision) { revision_ = revision; }
    void set_terrain_revision(std::uint64_t revision) { terrain_revision_ = revision; }
    void set_seed(std::uint32_t seed) { seed_ = seed; }
};
}
