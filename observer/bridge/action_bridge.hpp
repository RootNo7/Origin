#pragma once
#include "engine/simulation/simulation.hpp"
#include <filesystem>
namespace origin {
class ActionBridge {
public:
    static std::size_t process(Simulation&, const std::filesystem::path&, const std::filesystem::path&);
};
}
