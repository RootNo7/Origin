#pragma once
#include "engine/simulation/simulation.hpp"
#include <filesystem>
namespace origin { class StateBridge { public: static bool write(const Simulation&, const std::filesystem::path&); }; }
