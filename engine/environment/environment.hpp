#pragma once

#include "engine/world/world.hpp"

namespace origin {

class Environment {
public:
    void reset();
    void step(World& world, double simulation_seconds, double dt_seconds);

    [[nodiscard]] double sunlight() const { return sunlight_; }
    [[nodiscard]] double global_temperature_k() const { return global_temperature_k_; }

private:
    double sunlight_{1.0};
    double global_temperature_k_{288.15};
};

}
