#include "engine/time/simulation_clock.hpp"

#include <algorithm>

namespace origin {

void SimulationClock::reset(double fixed_dt_seconds) {
    tick_ = 0;
    seconds_ = 0.0;
    fixed_dt_seconds_ = fixed_dt_seconds > 0.0 ? fixed_dt_seconds : (1.0 / 60.0);
    speed_ = 1.0;
    paused_ = false;
}

void SimulationClock::advance() {
    if (paused_) {
        return;
    }
    seconds_ += fixed_dt_seconds_ * speed_;
    ++tick_;
}

void SimulationClock::set_speed(double multiplier) {
    speed_ = std::max(0.0, multiplier);
}

void SimulationClock::pause(bool paused) {
    paused_ = paused;
}

}

void origin::SimulationClock::restore(std::uint64_t tick, double seconds, double fixed_dt_seconds, double speed, bool paused) {
    tick_ = tick;
    seconds_ = seconds;
    fixed_dt_seconds_ = fixed_dt_seconds > 0.0 ? fixed_dt_seconds : (1.0 / 60.0);
    speed_ = std::max(0.0, speed);
    paused_ = paused;
}
