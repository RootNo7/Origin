#include "engine/chemistry/chemistry_world.hpp"

namespace origin {

WaterPhase water_phase(double temperature_kelvin, double pressure_kpa) {
    if (pressure_kpa <= 0.0) return WaterPhase::Vapor;
    if (temperature_kelvin <= 273.15) return WaterPhase::Ice;
    if (temperature_kelvin >= 373.15) return WaterPhase::Vapor;
    return WaterPhase::Liquid;
}

const char* water_phase_name(WaterPhase phase) {
    switch (phase) {
        case WaterPhase::None: return "none";
        case WaterPhase::Ice: return "ice";
        case WaterPhase::Liquid: return "liquid";
        case WaterPhase::Vapor: return "vapor";
    }
    return "unknown";
}

void ChemistryWorld::update(World& world) const {
    // Current implementation keeps chemistry deliberately small: it
    // establishes a physically meaningful water phase classification.
    // Molecular reactions will be added here when the required
    // matter/chemical representation is defined.
    for (auto& column : world.columns()) {
        if (column.water_depth_m <= 0.0) continue;
        const auto phase = water_phase(column.temperature_k);
        if (phase == WaterPhase::Ice) {
            // Keep the water mass; material_at() can later distinguish phase.
            continue;
        }
        if (phase == WaterPhase::Vapor) {
            // Humidity represents atmospheric water availability.
            column.humidity = 1.0;
        }
    }
}

}
