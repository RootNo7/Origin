#pragma once
#include "engine/entities/entity.hpp"
#include <algorithm>
#include <unordered_map>
#include <vector>
namespace origin {
class EntityRegistry {
    EntityId next_ = 1;
    std::vector<Entity> entities_;
    std::unordered_map<EntityId, std::size_t> index_;
public:
    EntityId create(std::string name, Material material, const RigidBody& body);
    bool insert_restored(const Entity& entity);
    Entity* find(EntityId id);
    const Entity* find(EntityId id) const;
    void clear();
    std::size_t size() const { return entities_.size(); }
    EntityId next_id() const { return next_; }
    const std::vector<Entity>& all() const { return entities_; }
    std::vector<Entity>& all() { return entities_; }
};
}
