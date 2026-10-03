#include "engine/environment/environment.hpp"

#include <algorithm>
#include <cmath>

namespace origin {

void Environment::reset() {
    sunlight_ = 1.0;
    global_temperature_k_ = 288.15;
}

void Environment::step(World& world, double simulation_seconds, double dt_seconds) {
    if (dt_seconds <= 0.0) return;

    constexpr double day_seconds = 86400.0;
    const double phase = std::fmod(simulation_seconds, day_seconds) / day_seconds;
    const double solar_angle = phase * 2.0 * 3.141592653589793;

    sunlight_ = std::max(0.0, std::sin(solar_angle - 3.141592653589793 / 2.0));
    global_temperature_k_ = 286.0 + 3.0 * sunlight_;

    for (std::size_t x = 0; x < world.width(); ++x) {
        auto& column = world.columns()[x];
        const double target = global_temperature_k_ +
            4.0 * std::sin(static_cast<double>(x) * 0.04);
        const double response = std::clamp(dt_seconds / 120.0, 0.0, 1.0);
        column.temperature_k += (target - column.temperature_k) * response;
    }
}

}
