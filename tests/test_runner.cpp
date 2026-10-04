#include "engine/simulation/simulation.hpp"
#include <cassert>
int main(){origin::Simulation s(80,28);s.initialize(7);assert(s.world().width()==80);assert(s.entities().size()==2);s.step();assert(s.clock().tick()==1);assert(s.entities().all()[0].body.position.y<22);return 0;}
