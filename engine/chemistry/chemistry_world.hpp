#pragma once

#include "engine/world/world.hpp"

namespace origin {

enum class WaterPhase {
    None,
    Ice,
    Liquid,
    Vapor
};

[[nodiscard]] WaterPhase water_phase(double temperature_kelvin, double pressure_kpa = 101.325);
[[nodiscard]] const char* water_phase_name(WaterPhase phase);

class ChemistryWorld {
public:
    void update(World& world) const;
};

}
