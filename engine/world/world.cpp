#include "engine/world/world.hpp"

#include <algorithm>
#include <cmath>
#include <random>

namespace origin {

MaterialProperties material_properties(Material material) {
    switch (material) {
        case Material::Air:   return {1.225, 0.0, 0.0};
        case Material::Water: return {1000.0, 0.1, 0.03};
        case Material::Soil:  return {1600.0, 0.15, 0.8};
        case Material::Rock:  return {2700.0, 0.25, 0.95};
        case Material::Ice:   return {917.0, 0.2, 0.4};
    }
    return {};
}

const char* material_name(Material material) {
    switch (material) {
        case Material::Air: return "air";
        case Material::Water: return "water";
        case Material::Soil: return "soil";
        case Material::Rock: return "rock";
        case Material::Ice: return "ice";
    }
    return "unknown";
}

World::World(std::size_t width, std::size_t height)
    : width_(width), height_(height), columns_(width) {}

void World::generate_vearth(std::uint32_t seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> noise(-1.2, 1.2);

    const double max_ground = static_cast<double>(height_) - 2.0;

    for (std::size_t x = 0; x < width_; ++x) {
        const double xf = static_cast<double>(x);
        const double broad = std::sin(xf * 0.075) * 2.5;
        const double small = std::sin(xf * 0.23) * 0.8;
        const double random = noise(rng);
        const double ground = std::clamp(7.0 + broad + small + random, 2.0, max_ground);

        auto& column = columns_[x];
        column.ground_height_m = ground;
        column.temperature_k = 286.0 + 6.0 * std::sin(xf / static_cast<double>(width_) * 3.14159);
        column.water_depth_m = ground < sea_level_m_ ? (sea_level_m_ - ground) : 0.0;
        column.humidity = column.water_depth_m > 0.0 ? 0.85 : 0.45;
    }
}

double World::ground_height(double x) const {
    if (columns_.empty()) return 0.0;
    const double clamped = std::clamp(x, 0.0, static_cast<double>(width_ - 1));
    const auto left = static_cast<std::size_t>(std::floor(clamped));
    const auto right = std::min(left + 1, width_ - 1);
    const double t = clamped - static_cast<double>(left);
    return columns_[left].ground_height_m * (1.0 - t) + columns_[right].ground_height_m * t;
}

Material World::material_at(double x, double y) const {
    if (!inside(x, y) || columns_.empty()) return Material::Air;
    const auto index = static_cast<std::size_t>(std::clamp(std::floor(x), 0.0, static_cast<double>(width_ - 1)));
    const auto& column = columns_[index];

    if (y < column.ground_height_m) {
        return column.ground_height_m <= sea_level_m_ ? Material::Water : Material::Soil;
    }
    return Material::Air;
}

bool World::inside(double x, double y) const {
    return x >= 0.0 && x < static_cast<double>(width_) &&
           y >= 0.0 && y < static_cast<double>(height_);
}

}
