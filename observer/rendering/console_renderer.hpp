#pragma once

#include "engine/simulation/simulation.hpp"

#include <ostream>

namespace origin {

class ConsoleRenderer {
public:
    static void render(const Simulation& simulation, std::ostream& out);
};

}
