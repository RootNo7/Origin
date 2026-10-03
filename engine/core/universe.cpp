#include "universe.h"
#include <algorithm>

namespace origin::core {

Universe::Universe() = default;

Universe::~Universe() {
    // Shutdown all systems in reverse order
    for (auto it = systems_.rbegin(); it != systems_.rend(); ++it) {
        (*it)->shutdown(this);
    }
}

void Universe::initialize(uint64_t seed) {
    if (initialized_) return;
    
    state_.set_seed(seed != 0 ? seed : std::random_device{}());
    rng_.seed(state_.seed());
    initialized_ = true;
}

void Universe::step(double dt) {
    if (paused_) return;
    
    double scaled_dt = dt * time_scale_;
    state_.advance_time(scaled_dt);
    state_.increment_tick();
    
    // Update all systems
    for (auto& system : systems_) {
        system->update(this, scaled_dt);
    }
}

void Universe::run_for(double duration, double dt) {
    double elapsed = 0.0;
    while (elapsed < duration && !paused_) {
        step(dt);
        elapsed += dt;
    }
}

void Universe::pause() {
    if (!paused_) {
        paused_ = true;
        for (auto& system : systems_) {
            system->on_pause(this);
        }
    }
}

void Universe::resume() {
    if (paused_) {
        paused_ = false;
        for (auto& system : systems_) {
            system->on_resume(this);
        }
    }
}

bool Universe::is_paused() const noexcept {
    return paused_;
}

void Universe::set_time_scale(double scale) noexcept {
    time_scale_ = std::max(0.0, scale);
}

double Universe::time_scale() const noexcept {
    return time_scale_;
}

void Universe::set_deterministic(bool deterministic) noexcept {
    deterministic_ = deterministic;
}

bool Universe::is_deterministic() const noexcept {
    return deterministic_;
}

} // namespace origin::core