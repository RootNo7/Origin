#include "engine/time/simulation_clock.hpp"
#include <cmath>
#include <limits>

namespace origin {

CalendarTime calendar_from_seconds(double seconds) {
    if (!std::isfinite(seconds) || seconds < 0.0) seconds = 0.0;
    const double max_seconds = static_cast<double>(std::numeric_limits<std::uint64_t>::max());
    const auto total = static_cast<std::uint64_t>(std::min(seconds, max_seconds));

    constexpr std::uint64_t kSecondsPerMinute = 60;
    constexpr std::uint64_t kMinutesPerHour = 60;
    constexpr std::uint64_t kHoursPerDay = 24;
    constexpr std::uint64_t kDaysPerWeek = 7;
    constexpr std::uint64_t kDaysPerMonth = 30;
    constexpr std::uint64_t kMonthsPerYear = 12;
    constexpr std::uint64_t kSecondsPerHour = kSecondsPerMinute * kMinutesPerHour;
    constexpr std::uint64_t kSecondsPerDay = kSecondsPerHour * kHoursPerDay;
    constexpr std::uint64_t kDaysPerYear = kDaysPerMonth * kMonthsPerYear;
    constexpr std::uint64_t kSecondsPerYear = kSecondsPerDay * kDaysPerYear;

    CalendarTime result;
    result.total_seconds = total;
    const auto year = total / kSecondsPerYear;
    const auto within_year = total % kSecondsPerYear;
    const auto day_of_year = within_year / kSecondsPerDay;
    const auto within_day = within_year % kSecondsPerDay;

    result.year = year;
    result.month = day_of_year / kDaysPerMonth;
    result.day_of_month = day_of_year % kDaysPerMonth;
    result.day = total / kSecondsPerDay;
    result.week = result.day / kDaysPerWeek;
    result.day_of_week = result.day % kDaysPerWeek;
    result.hour = within_day / kSecondsPerHour;
    result.minute = (within_day % kSecondsPerHour) / kSecondsPerMinute;
    result.second = within_day % kSecondsPerMinute;
    return result;
}

void SimulationClock::reset(double d) {
    tick_ = 0;
    sec_ = 0.0;
    dt_ = d > 0.0 && std::isfinite(d) ? d : 1.0 / 30.0;
    speed_ = 1.0;
    paused_ = false;
}

void SimulationClock::advance() {
    if (paused_) return;
    sec_ += dt_ * speed_;
    ++tick_;
}

void SimulationClock::restore(std::uint64_t t, double s, double d, double sp, bool p) {
    tick_ = t;
    sec_ = std::max(0.0, std::isfinite(s) ? s : 0.0);
    dt_ = d > 0.0 && std::isfinite(d) ? d : 1.0 / 30.0;
    speed_ = std::max(0.0, std::isfinite(sp) ? sp : 1.0);
    paused_ = p;
}
}
