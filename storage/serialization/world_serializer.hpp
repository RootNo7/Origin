#pragma once
#include "engine/simulation/simulation.hpp"
#include <filesystem>

namespace origin {
class WorldSerializer {
public:
    static bool save(const Simulation&, const std::filesystem::path&);
    static bool load(Simulation&, const std::filesystem::path&);
};
}
