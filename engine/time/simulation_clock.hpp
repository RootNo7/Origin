#pragma once

#include <cstdint>

namespace origin {

class SimulationClock {
public:
    void reset(double fixed_dt_seconds = 1.0 / 60.0);
    void advance();
    void set_speed(double multiplier);
    void pause(bool paused);
    void restore(std::uint64_t tick, double seconds, double fixed_dt_seconds, double speed, bool paused);

    [[nodiscard]] std::uint64_t tick() const { return tick_; }
    [[nodiscard]] double seconds() const { return seconds_; }
    [[nodiscard]] double fixed_dt() const { return fixed_dt_seconds_; }
    [[nodiscard]] double speed() const { return speed_; }
    [[nodiscard]] bool paused() const { return paused_; }

private:
    std::uint64_t tick_{0};
    double seconds_{0.0};
    double fixed_dt_seconds_{1.0 / 60.0};
    double speed_{1.0};
    bool paused_{false};
};

}
