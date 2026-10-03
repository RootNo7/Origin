#pragma once

#include "simulation_clock.h"
#include <string>

namespace origin::time {

/**
 * @brief Time conversion utilities.
 * 
 * Provides conversion between simulation time units and human-readable formats.
 */
class TimeConversion {
public:
    // Simulation time to human units
    [[nodiscard]] static double ticks_to_seconds(uint64_t ticks) noexcept {
        return ticks * TimeConstants::SECONDS_PER_TICK;
    }
    
    [[nodiscard]] static uint64_t seconds_to_ticks(double seconds) noexcept {
        return static_cast<uint64_t>(seconds * TimeConstants::TICKS_PER_SECOND);
    }
    
    [[nodiscard]] static double ticks_to_minutes(uint64_t ticks) noexcept {
        return ticks_to_seconds(ticks) / TimeConstants::SECONDS_PER_MINUTE;
    }
    
    [[nodiscard]] static double ticks_to_hours(uint64_t ticks) noexcept {
        return ticks_to_seconds(ticks) / TimeConstants::SECONDS_PER_HOUR;
    }
    
    [[nodiscard]] static double ticks_to_days(uint64_t ticks) noexcept {
        return ticks_to_seconds(ticks) / TimeConstants::SECONDS_PER_DAY;
    }
    
    [[nodiscard]] static double ticks_to_years(uint64_t ticks) noexcept {
        return ticks_to_seconds(ticks) / TimeConstants::SECONDS_PER_YEAR;
    }
    
    // Human units to simulation time
    [[nodiscard]] static uint64_t minutes_to_ticks(double minutes) noexcept {
        return seconds_to_ticks(minutes * TimeConstants::SECONDS_PER_MINUTE);
    }
    
    [[nodiscard]] static uint64_t hours_to_ticks(double hours) noexcept {
        return seconds_to_ticks(hours * TimeConstants::SECONDS_PER_HOUR);
    }
    
    [[nodiscard]] static uint64_t days_to_ticks(double days) noexcept {
        return seconds_to_ticks(days * TimeConstants::SECONDS_PER_DAY);
    }
    
    [[nodiscard]] static uint64_t years_to_ticks(double years) noexcept {
        return seconds_to_ticks(years * TimeConstants::SECONDS_PER_YEAR);
    }
    
    // Format time as human-readable string
    [[nodiscard]] static std::string format_duration(double seconds);
    [[nodiscard]] static std::string format_ticks(uint64_t ticks);
    
    // Parse human-readable time to ticks
    [[nodiscard]] static uint64_t parse_duration(const std::string& str);
};

} // namespace origin::time