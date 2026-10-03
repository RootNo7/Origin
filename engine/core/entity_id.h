#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace origin::core {

/**
 * @brief Stable, unique identifier for entities in the universe.
 * 
 * Entity IDs are the single source of truth for entity identity.
 * They persist across simulation saves and loads.
 * Never use array indices or pointers as entity identifiers.
 */
class EntityId {
public:
    using UnderlyingType = uint64_t;
    
    constexpr EntityId() noexcept : id_(0) {}
    explicit constexpr EntityId(UnderlyingType id) noexcept : id_(id) {}
    
    [[nodiscard]] constexpr UnderlyingType value() const noexcept { return id_; }
    [[nodiscard]] constexpr bool is_valid() const noexcept { return id_ != 0; }
    [[nodiscard]] constexpr bool operator==(const EntityId& other) const noexcept { return id_ == other.id_; }
    [[nodiscard]] constexpr bool operator!=(const EntityId& other) const noexcept { return id_ != other.id_; }
    [[nodiscard]] constexpr bool operator<(const EntityId& other) const noexcept { return id_ < other.id_; }
    
    [[nodiscard]] std::string to_string() const;
    
    static EntityId invalid() noexcept { return EntityId(0); }
    
private:
    UnderlyingType id_;
};

/**
 * @brief Generates new unique entity IDs.
 * 
 * Uses a simple counter for now. In the future, this could use
 * a more sophisticated scheme (e.g., UUID, distributed ID generation).
 */
class EntityIdGenerator {
public:
    explicit EntityIdGenerator(EntityId::UnderlyingType start = 1) noexcept : next_id_(start) {}
    
    [[nodiscard]] EntityId generate() noexcept {
        return EntityId(next_id_++);
    }
    
    void set_next_id(EntityId::UnderlyingType id) noexcept { next_id_ = id; }
    [[nodiscard]] EntityId::UnderlyingType next_id() const noexcept { return next_id_; }
    
private:
    EntityId::UnderlyingType next_id_;
};

} // namespace origin::core

// Hash specialization for unordered containers
namespace std {
template<>
struct hash<origin::core::EntityId> {
    size_t operator()(const origin::core::EntityId& id) const noexcept {
        return std::hash<origin::core::EntityId::UnderlyingType>{}(id.value());
    }
};
}