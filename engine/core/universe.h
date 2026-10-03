#pragma once

#include "universe_state.h"
#include "system.h"
#include <memory>
#include <vector>
#include <chrono>
#include <random>

namespace origin::core {

class Universe {
public:
    Universe();
    ~Universe();
    
    // Non-copyable, movable
    Universe(const Universe&) = delete;
    Universe& operator=(const Universe&) = delete;
    Universe(Universe&&) = default;
    Universe& operator=(Universe&&) = default;
    
    // Initialize the universe with a seed
    void initialize(uint64_t seed = 0);
    
    // Run a single simulation step
    void step(double dt);
    
    // Run simulation for a duration
    void run_for(double duration, double dt);
    
    // Run until a condition is met
    template<typename Predicate>
    void run_until(Predicate&& pred, double dt) {
        while (!pred(*this)) {
            step(dt);
        }
    }
    
    // Pause/resume
    void pause();
    void resume();
    [[nodiscard]] bool is_paused() const noexcept;
    
    // Simulation speed control
    void set_time_scale(double scale) noexcept;
    [[nodiscard]] double time_scale() const noexcept;
    
    // Access to universe state
    [[nodiscard]] UniverseState& state() noexcept { return state_; }
    [[nodiscard]] const UniverseState& state() const noexcept { return state_; }
    
    // System management
    template<typename T, typename... Args>
    T* add_system(Args&&... args) {
        static_assert(std::is_base_of_v<System, T>, "T must derive from System");
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = system.get();
        system->initialize(this);
        systems_.push_back(std::move(system));
        // Sort by priority
        std::sort(systems_.begin(), systems_.end(),
            [](const auto& a, const auto& b) {
                return a->priority() < b->priority();
            });
        return raw;
    }
    
    template<typename T>
    T* get_system() {
        static_assert(std::is_base_of_v<System, T>, "T must derive from System");
        for (auto& system : systems_) {
            if (auto* typed = dynamic_cast<T*>(system.get())) {
                return typed;
            }
        }
        return nullptr;
    }
    
    // Random number generation (deterministic)
    [[nodiscard]] std::mt19937_64& rng() noexcept { return rng_; }
    
    // Determinism control
    void set_deterministic(bool deterministic) noexcept;
    [[nodiscard]] bool is_deterministic() const noexcept;
    
private:
    UniverseState state_;
    std::vector<std::unique_ptr<System>> systems_;
    std::mt19937_64 rng_;
    bool paused_ = false;
    double time_scale_ = 1.0;
    bool deterministic_ = true;
    bool initialized_ = false;
};

} // namespace origin::core