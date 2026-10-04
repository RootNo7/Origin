#pragma once
#include "engine/core/types.hpp"
#include <cstdint>
#include <vector>

namespace origin {

enum class Material { Air, Water, Soil, Rock, Ice };

struct TerrainCell {
    double ground_height_m = 0.0;
    double temperature_k = 288.15;
    double water_depth_m = 0.0;
    double humidity = 0.5;
};

class World {
    std::size_t width_ = 0;
    std::size_t depth_ = 0;
    double sea_level_m_ = 8.0;
    std::vector<TerrainCell> cells_;
    std::uint64_t revision_ = 0;

    std::size_t index(std::size_t x, std::size_t z) const { return z * width_ + x; }

public:
    World(std::size_t width, std::size_t depth)
        : width_(width), depth_(depth), cells_(width * depth) {}

    void generate_vearth(std::uint32_t seed);

    double ground_height(double x, double z) const;
    const TerrainCell& cell(std::size_t x, std::size_t z) const { return cells_[index(x, z)]; }
    TerrainCell& cell(std::size_t x, std::size_t z) { return cells_[index(x, z)]; }

    std::size_t width() const { return width_; }
    std::size_t depth() const { return depth_; }
    // Kept as an intentional compatibility alias for older observers/tools.
    std::size_t height() const { return depth_; }
    double sea_level_m() const { return sea_level_m_; }
    std::uint64_t revision() const { return revision_; }

    const std::vector<TerrainCell>& cells() const { return cells_; }
    std::vector<TerrainCell>& cells() { return cells_; }
};
}
