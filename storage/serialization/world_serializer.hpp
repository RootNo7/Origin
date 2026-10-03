#pragma once

#include "engine/simulation/simulation.hpp"

#include <filesystem>
#include <string>

namespace origin {

class WorldSerializer {
public:
    static bool save(const Simulation& simulation, const std::filesystem::path& path, std::string& error);
    static bool load(Simulation& simulation, const std::filesystem::path& path, std::string& error);
};

}
