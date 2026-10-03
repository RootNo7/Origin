#pragma once

#include "entity_id.h"
#include "component.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <typeindex>

namespace origin::core {

class Entity {
public:
    explicit Entity(EntityId id) noexcept : id_(id) {}
    virtual ~Entity() = default;
    
    // Non-copyable, movable
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    Entity(Entity&&) = default;
    Entity& operator=(Entity&&) = default;
    
    [[nodiscard]] EntityId id() const noexcept { return id_; }
    [[nodiscard]] bool is_active() const noexcept { return active_; }
    void set_active(bool active) noexcept { active_ = active; }
    
    // Component management
    template<typename T, typename... Args>
    T* add_component(Args&&... args) {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw_ptr = component.get();
        components_[T::type_index_static()] = std::move(component);
        raw_ptr->on_add(this);
        return raw_ptr;
    }
    
    template<typename T>
    T* get_component() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        auto it = components_.find(T::type_index_static());
        return it != components_.end() ? static_cast<T*>(it->second.get()) : nullptr;
    }
    
    template<typename T>
    const T* get_component() const {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        auto it = components_.find(T::type_index_static());
        return it != components_.end() ? static_cast<const T*>(it->second.get()) : nullptr;
    }
    
    template<typename T>
    bool has_component() const noexcept {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        return components_.find(T::type_index_static()) != components_.end();
    }
    
    template<typename T>
    void remove_component() {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        auto it = components_.find(T::type_index_static());
        if (it != components_.end()) {
            it->second->on_remove(this);
            components_.erase(it);
        }
    }
    
    [[nodiscard]] std::vector<std::type_index> component_types() const {
        std::vector<std::type_index> types;
        types.reserve(components_.size());
        for (const auto& [type, _] : components_) {
            types.push_back(type);
        }
        return types;
    }
    
private:
    EntityId id_;
    bool active_ = true;
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components_;
};

} // namespace origin::core