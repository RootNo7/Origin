#pragma once

#include <cstdint>
#include <typeindex>
#include <memory>
#include <string>

namespace origin::core {

class Entity;

/**
 * @brief Base class for all components.
 * 
 * Components are data-only structures that attach to entities.
 * They should not contain complex logic - that belongs in Systems.
 * Components must be trivially copyable where possible for performance.
 */
class Component {
public:
    virtual ~Component() = default;
    
    // Prevent slicing
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
    Component(Component&&) = default;
    Component& operator=(Component&&) = default;
    
    [[nodiscard]] virtual std::type_index type() const noexcept = 0;
    [[nodiscard]] virtual std::string type_name() const noexcept = 0;
    
    // Called when component is added to an entity
    virtual void on_add(Entity* entity) {}
    
    // Called when component is removed from an entity
    virtual void on_remove(Entity* entity) {}
    
protected:
    Component() = default;
};

/**
 * @brief CRTP base for statically typed components.
 * 
 * Provides automatic type_index and type_name implementation.
 */
template<typename Derived>
class TypedComponent : public Component {
public:
    [[nodiscard]] std::type_index type() const noexcept override {
        return std::type_index(typeid(Derived));
    }
    
    [[nodiscard]] std::string type_name() const noexcept override {
        return typeid(Derived).name();
    }
    
    static std::type_index type_index_static() noexcept {
        return std::type_index(typeid(Derived));
    }
};

} // namespace origin::core