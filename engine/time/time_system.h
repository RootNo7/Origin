#pragma once

#include "simulation_clock.h"
#include "../core/system.h"
#include <functional>

namespace origin::time {

/**
 * @brief Time system - manages the simulation clock and time scaling.
 * 
 * This system is responsible for advancing the simulation clock each tick
 * and providing time-related services to other systems.
 */
class TimeSystem : public origin::core::System {
public:
    TimeSystem() = default;
    ~TimeSystem() override = default;
    
    [[nodiscard]] std::string name() const noexcept override { return "TimeSystem"; }
    [[nodiscard]] int priority() const noexcept override { return -1000; }  // Run first
    
    void initialize(origin::core::Universe* universe) override;
    void update(origin::core::Universe* universe, double dt) override;
    void on_pause(origin::core::Universe* universe) override;
    void on_resume(origin::core::Universe* universe) override;
    
    // Time scale control
    void set_time_scale(double scale) noexcept;
    [[nodiscard]] double time_scale() const noexcept { return time_scale_; }
    
    // Fixed timestep access
    [[nodiscard]] double fixed_timestep() const noexcept { return fixed_timestep_; }
    void set_fixed_timestep(double timestep) noexcept { fixed_timestep_ = std::max(0.0001, timestep); }
    
    // Clock access
    [[nodiscard]] SimulationClock& clock() noexcept { return clock_; }
    [[nodiscard]] const SimulationClock& clock() const noexcept { return clock_; }
    
    // Callbacks for time events
    using TimeCallback = std::function<void(const SimulationClock&)>;
    void on_day_changed(TimeCallback callback) { day_changed_callbacks_.push_back(std::move(callback)); }
    void on_year_changed(TimeCallback callback) { year_changed_callbacks_.push_back(std::move(callback)); }
    
private:
    SimulationClock clock_;
    double time_scale_ = 1.0;
    double fixed_timestep_ = TimeConstants::FIXED_TIMESTEP;
    bool was_paused_ = false;
    
    // Track calendar changes
    uint64_t last_day_ = 0;
    uint64_t last_year_ = 0;
    std::vector<TimeCallback> day_changed_callbacks_;
    std::vector<TimeCallback> year_changed_callbacks_;
    
    void check_calendar_transitions();
};

} // namespace origin::time