#include "time_system.h"
#include "../core/universe.h"

namespace origin::time {

void TimeSystem::initialize(origin::core::Universe* universe) {
    // Sync with universe state
    clock_.set_tick(universe->state().tick());
    clock_.set_simulation_time(universe->state().simulation_time());
    
    // Initialize calendar tracking
    auto cal = clock_.calendar_time();
    last_day_ = cal.day_of_year + cal.year * 365;
    last_year_ = cal.year;
}

void TimeSystem::update(origin::core::Universe* universe, double dt) {
    // Advance clock by fixed timestep (scaled)
    double scaled_dt = fixed_timestep_ * time_scale_;
    clock_.advance_ticks(1);
    
    // Sync universe state
    universe->state().set_tick(clock_.tick());
    universe->state().set_simulation_time(clock_.simulation_time());
    
    // Check for calendar transitions
    check_calendar_transitions();
}

void TimeSystem::on_pause(origin::core::Universe* universe) {
    was_paused_ = true;
}

void TimeSystem::on_resume(origin::core::Universe* universe) {
    was_paused_ = false;
}

void TimeSystem::set_time_scale(double scale) noexcept {
    time_scale_ = std::max(0.0, scale);
}

void TimeSystem::check_calendar_transitions() {
    auto cal = clock_.calendar_time();
    uint64_t current_day = cal.day_of_year + cal.year * 365;
    uint64_t current_year = cal.year;
    
    if (current_day != last_day_) {
        for (auto& callback : day_changed_callbacks_) {
            callback(clock_);
        }
        last_day_ = current_day;
    }
    
    if (current_year != last_year_) {
        for (auto& callback : year_changed_callbacks_) {
            callback(clock_);
        }
        last_year_ = current_year;
    }
}

} // namespace origin::time