#pragma once
#include <algorithm>
#include <cstdint>

namespace origin {

struct CalendarTime {
    std::uint64_t total_seconds = 0;
    std::uint64_t second = 0;
    std::uint64_t minute = 0;
    std::uint64_t hour = 0;
    std::uint64_t day = 0;
    std::uint64_t week = 0;
    std::uint64_t month = 0;
    std::uint64_t year = 0;
    std::uint64_t day_of_month = 0;
    std::uint64_t day_of_week = 0;
};

// Origin's initial experimental calendar is intentionally simple and deterministic:
// 60 s/min, 60 min/hour, 24 h/day, 7 days/week, 30 days/month, 12 months/year.
CalendarTime calendar_from_seconds(double seconds);

class SimulationClock {
    std::uint64_t tick_ = 0;
    double sec_ = 0.0;
    double dt_ = 1.0 / 30.0;
    double speed_ = 1.0;
    bool paused_ = false;

public:
    void reset(double dt = 1.0 / 30.0);
    void advance();
    void restore(std::uint64_t tick, double seconds, double dt, double speed, bool paused);

    std::uint64_t tick() const { return tick_; }
    double seconds() const { return sec_; }
    double fixed_dt() const { return dt_; }
    double speed() const { return speed_; }
    bool paused() const { return paused_; }
};
}
