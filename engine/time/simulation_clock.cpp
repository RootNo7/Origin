#include "engine/time/simulation_clock.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace origin {
namespace {
constexpr double kDefaultDt = 1.0 / 30.0;
constexpr double kMinDt = 1e-6;
constexpr double kMaxDt = 0.25;
constexpr double kMaxSpeed = 16.0;
}

CalendarTime calendar_from_seconds(double seconds) {
    if (!std::isfinite(seconds) || seconds < 0.0) seconds = 0.0;
    const long double max_total = static_cast<long double>(std::numeric_limits<std::uint64_t>::max());
    const long double safe_seconds = static_cast<long double>(seconds);
    const auto total = safe_seconds >= max_total
        ? std::numeric_limits<std::uint64_t>::max()
        : static_cast<std::uint64_t>(safe_seconds);

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
    dt_ = std::clamp(std::isfinite(d) && d > 0.0 ? d : kDefaultDt, kMinDt, kMaxDt);
    speed_ = 1.0;
    paused_ = false;
}

void SimulationClock::advance() {
    if (paused_ || tick_ == std::numeric_limits<std::uint64_t>::max()) return;
    const double delta = dt_ * speed_;
    if (!std::isfinite(delta) || delta < 0.0) return;
    const double next_seconds = sec_ + delta;
    if (!std::isfinite(next_seconds) || next_seconds < sec_) {
        sec_ = std::numeric_limits<double>::max();
        ++tick_;
        return;
    }
    sec_ = next_seconds;
    ++tick_;
}

void SimulationClock::restore(std::uint64_t t, double s, double d, double sp, bool p) {
    tick_ = t;
    sec_ = std::max(0.0, std::isfinite(s) ? s : 0.0);
    dt_ = std::clamp(std::isfinite(d) && d > 0.0 ? d : kDefaultDt, kMinDt, kMaxDt);
    speed_ = std::clamp(std::isfinite(sp) && sp >= 0.0 ? sp : 1.0, 0.0, kMaxSpeed);
    paused_ = p;
}
}
