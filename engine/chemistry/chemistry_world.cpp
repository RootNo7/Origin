#include "engine/chemistry/chemistry_world.hpp"

namespace origin {
void ChemistryWorld::update(World& world) const {
    for (std::size_t z = 0; z < world.depth(); ++z) {
        for (std::size_t x = 0; x < world.width(); ++x) {
            auto& cell = world.cell(x, z);
            if (cell.water_depth_m > 0.0 && water_phase(cell.temperature_k) == WaterPhase::Vapor)
                cell.humidity = 1.0;
        }
    }
}
}
