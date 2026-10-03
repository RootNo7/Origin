#pragma once

#include "entity_id.h"
#include "entity.h"
#include "entity_id_generator.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <chrono>

namespace origin::core {

/**
 * @brief Complete state of the universe at a point in time.
 * 
 * This is the authoritative simulation state. All systems read from
 * and write to this state. It must be serializable for persistence.
 */
class UniverseState {
public:
    UniverseState() = default;
    ~UniverseState() = default;
    
    // Non-copyable, movable
    UniverseState(const UniverseState&) = delete;
    UniverseState& operator=(const UniverseState&) = delete;
    UniverseState(UniverseState&&) = default;
    UniverseState& operator=(UniverseState&&) = default;
    
    // Entity management
    Entity* create_entity() {
        EntityId id = id_generator_.generate();
        auto entity = std::make_unique<Entity>(id);
        Entity* raw = entity.get();
        entities_[id] = std::move(entity);
        return raw;
    }
    
    Entity* create_entity(EntityId id) {
        auto entity = std::make_unique<Entity>(id);
        Entity* raw = entity.get();
        entities_[id] = std::move(entity);
        id_generator_.set_next_id(std::max(id_generator_.next_id(), id.value() + 1));
        return raw;
    }
    
    void destroy_entity(EntityId id) {
        auto it = entities_.find(id);
        if (it != entities_.end()) {
            entities_.erase(it);
        }
    }
    
    [[nodiscard]] Entity* get_entity(EntityId id) {
        auto it = entities_.find(id);
        return it != entities_.end() ? it->second.get() : nullptr;
    }
    
    [[nodiscard]] const Entity* get_entity(EntityId id) const {
        auto it = entities_.find(id);
        return it != entities_.end() ? it->second.get() : nullptr;
    }
    
    [[nodiscard]] bool has_entity(EntityId id) const noexcept {
        return entities_.find(id) != entities_.end();
    }
    
    [[nodiscard]] const std::unordered_map<EntityId, std::unique_ptr<Entity>>& entities() const noexcept {
        return entities_;
    }
    
    [[nodiscard]] std::vector<Entity*> active_entities() {
        std::vector<Entity*> result;
        result.reserve(entities_.size());
        for (auto& [id, entity] : entities_) {
            if (entity->is_active()) {
                result.push_back(entity.get());
            }
        }
        return result;
    }
    
    // Entity ID generator access (for persistence)
    [[nodiscard]] EntityIdGenerator& id_generator() noexcept { return id_generator_; }
    [[nodiscard]] const EntityIdGenerator& id_generator() const noexcept { return id_generator_; }
    
    // Simulation metadata
    [[nodiscard]] uint64_t tick() const noexcept { return tick_; }
    void set_tick(uint64_t tick) noexcept { tick_ = tick; }
    void increment_tick() noexcept { ++tick_; }
    
    [[nodiscard]] double simulation_time() const noexcept { return simulation_time_; }
    void set_simulation_time(double time) noexcept { simulation_time_ = time; }
    void advance_time(double dt) noexcept { simulation_time_ += dt; }
    
    [[nodiscard]] uint64_t seed() const noexcept { return seed_; }
    void set_seed(uint64_t seed) noexcept { seed_ = seed; }
    
    // Version for save format compatibility
    [[nodiscard]] uint32_t version() const noexcept { return version_; }
    void set_version(uint32_t version) noexcept { version_ = version; }
    
private:
    EntityIdGenerator id_generator_;
    std::unordered_map<EntityId, std::unique_ptr<Entity>> entities_;
    uint64_t tick_ = 0;
    double simulation_time_ = 0.0;
    uint64_t seed_ = 0;
    uint32_t version_ = 1;
};

} // namespace origin::core