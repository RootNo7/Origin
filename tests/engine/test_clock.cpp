#include "engine/time/simulation_clock.hpp"
#include <cassert>

void test_clock() {
    origin::SimulationClock clock;
    clock.reset(0.5);
    clock.advance();
    assert(clock.tick() == 1);
    assert(clock.seconds() == 0.5);

    clock.pause(true);
    clock.advance();
    assert(clock.tick() == 1);
}
