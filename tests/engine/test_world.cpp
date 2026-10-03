#include "engine/world/world.hpp"
#include <cassert>
#include <cmath>

void test_world_generation() {
    origin::World world(80, 28);
    world.generate_vearth(7);
    assert(world.width() == 80);
    assert(world.height() == 28);
    for (const auto& column : world.columns()) {
        assert(column.ground_height_m >= 2.0);
        assert(column.ground_height_m <= 26.0);
        assert(column.humidity >= 0.0 && column.humidity <= 1.0);
    }
}
