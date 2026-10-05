#include "engine/environment/environment.hpp"
#include <algorithm>
#include <cmath>

namespace origin {
void Environment::step(World& world, double time_seconds, double dt) {
    if (!std::isfinite(time_seconds) || !std::isfinite(dt) || dt <= 0.0) return;
    const double day_fraction = std::fmod(std::max(0.0, time_seconds), 86400.0) / 86400.0;
    sun_ = std::max(0.0, std::sin(day_fraction * 6.283185307179586 - 1.570796326794897));
    temp_ = 286.0 + 3.0 * sun_;

    for (std::size_t z = 0; z < world.depth(); ++z) {
        for (std::size_t x = 0; x < world.width(); ++x) {
            auto& cell = world.cell(x, z);
            const double target = temp_ + 4.0 * std::sin(x * 0.04) + 1.0 * std::cos(z * 0.025);
            cell.temperature_k += (target - cell.temperature_k) * std::clamp(dt / 120.0, 0.0, 1.0);
        }
    }
    world.mark_revision();
}
}
