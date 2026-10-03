#pragma once

#include <cstdint>
#include <chrono>
#include <string>

namespace origin::time {

/**
 * @brief Fundamental time constants for the simulation.
 * 
 * All time in the simulation is based on a fixed timestep for determinism.
 * Real-world time conversions are provided for convenience.
 */
struct TimeConstants {
    // Fixed simulation timestep (seconds)
    static constexpr double FIXED_TIMESTEP = 1.0 / 60.0;  // 60 Hz
    
    // Time conversions
    static constexpr double TICKS_PER_SECOND = 60.0;
    static constexpr double SECONDS_PER_TICK = 1.0 / 60.0;
    static constexpr double SECONDS_PER_MINUTE = 60.0;
    static constexpr double SECONDS_PER_HOUR = 3600.0;
    static constexpr double SECONDS_PER_DAY = 86400.0;
    static constexpr double SECONDS_PER_YEAR = 31557600.0;  // Julian year
    
    // Simulation time scale (can be changed at runtime)
    static constexpr double DEFAULT_TIME_SCALE = 1.0;
};

/**
 * @brief Simulation clock - the authoritative time source.
 * 
 * Tracks simulation ticks, simulation time, and provides
 * conversion to human-readable time units.
 */
class SimulationClock {
public:
    SimulationClock() = default;
    explicit SimulationClock(uint64_t initial_tick) noexcept : tick_(initial_tick) {}
    
    // Tick management
    [[nodiscard]] uint64_t tick() const noexcept { return tick_; }
    void set_tick(uint64_t tick) noexcept { tick_ = tick; }
    void increment_tick() noexcept { ++tick_; }
    void advance_ticks(uint64_t count) noexcept { tick_ += count; }
    
    // Simulation time (seconds)
    [[nodiscard]] double simulation_time() const noexcept {
        return tick_ * TimeConstants::SECONDS_PER_TICK;
    }
    
    void set_simulation_time(double seconds) noexcept {
        tick_ = static_cast<uint64_t>(seconds * TimeConstants::TICKS_PER_SECOND);
    }
    
    // Time conversions
    [[nodiscard]] double minutes() const noexcept { return simulation_time() / TimeConstants::SECONDS_PER_MINUTE; }
    [[nodiscard]] double hours() const noexcept { return simulation_time() / TimeConstants::SECONDS_PER_HOUR; }
    [[nodiscard]] double days() const noexcept { return simulation_time() / TimeConstants::SECONDS_PER_DAY; }
    [[nodiscard]] double years() const noexcept { return simulation_time() / TimeConstants::SECONDS_PER_YEAR; }
    
    // Human-readable time string
    [[nodiscard]] std::string to_string() const;
    
    // Calendar time (for worlds with day/night, seasons)
    struct CalendarTime {
        uint64_t year = 0;
        uint32_t day_of_year = 0;      // 0-364 (or 365 for leap)
        uint32_t hour = 0;             // 0-23
        uint32_t minute = 0;           // 0-59
        uint32_t second = 0;           // 0-59
        double fractional_second = 0.0;
    };
    
    [[nodiscard]] CalendarTime calendar_time(double seconds_per_day = TimeConstants::SECONDS_PER_DAY) const;
    
private:
    uint64_t tick_ = 0;
};

} // namespace origin::time