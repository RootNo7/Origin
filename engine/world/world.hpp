#pragma once

#include "engine/core/types.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace origin {

enum class Material : std::uint8_t {
    Air,
    Water,
    Soil,
    Rock,
    Ice
};

struct MaterialProperties {
    double density_kg_m3{};
    double restitution{};
    double friction{};
};

[[nodiscard]] MaterialProperties material_properties(Material material);
[[nodiscard]] const char* material_name(Material material);

struct Column {
    double ground_height_m{0.0};
    double temperature_k{288.15};
    double water_depth_m{0.0};
    double humidity{0.5};
};

class World {
public:
    World(std::size_t width, std::size_t height);

    void generate_vearth(std::uint32_t seed = 7);

    [[nodiscard]] std::size_t width() const { return width_; }
    [[nodiscard]] std::size_t height() const { return height_; }
    [[nodiscard]] double sea_level_m() const { return sea_level_m_; }
    [[nodiscard]] const std::vector<Column>& columns() const { return columns_; }
    [[nodiscard]] std::vector<Column>& columns() { return columns_; }

    [[nodiscard]] double ground_height(double x) const;
    [[nodiscard]] Material material_at(double x, double y) const;
    [[nodiscard]] bool inside(double x, double y) const;

private:
    std::size_t width_{};
    std::size_t height_{};
    double sea_level_m_{8.0};
    std::vector<Column> columns_;
};

}
