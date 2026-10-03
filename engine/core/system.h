#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <typeindex>

namespace origin::core {

class Universe;
class Entity;

/**
 * @brief Base class for all simulation systems.
 * 
 * Systems contain the logic that operates on entities with specific components.
 * They are updated each simulation tick in a defined order.
 */
class System {
public:
    virtual ~System() = default;
    
    // Non-copyable, movable
    System(const System&) = delete;
    System& operator=(const System&) = delete;
    System(System&&) = default;
    System& operator=(System&&) = default;
    
    [[nodiscard]] virtual std::string name() const noexcept = 0;
    [[nodiscard]] virtual int priority() const noexcept { return 0; }  // Lower = earlier
    
    // Called once when system is registered with universe
    virtual void initialize(Universe* universe) {}
    
    // Called each simulation tick
    virtual void update(Universe* universe, double dt) = 0;
    
    // Called when simulation is paused/resumed
    virtual void on_pause(Universe* universe) {}
    virtual void on_resume(Universe* universe) {}
    
    // Called before simulation shutdown
    virtual void shutdown(Universe* universe) {}
    
    // Component requirements for this system
    [[nodiscard]] virtual std::vector<std::type_index> required_components() const noexcept { return {}; }
    
protected:
    System() = default;
};

} // namespace origin::core