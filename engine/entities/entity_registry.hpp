#pragma once

#include "engine/entities/entity.hpp"

#include <cstddef>
#include <unordered_map>
#include <vector>

namespace origin {

class EntityRegistry {
public:
    EntityId create(std::string name, Material material, const RigidBody& body);
    bool insert_restored(const Entity& entity);
    Entity* find(EntityId id);
    const Entity* find(EntityId id) const;

    [[nodiscard]] std::vector<Entity>& all() { return entities_; }
    [[nodiscard]] const std::vector<Entity>& all() const { return entities_; }
    [[nodiscard]] std::size_t size() const { return entities_.size(); }

private:
    EntityId next_id_{1};
    std::vector<Entity> entities_;
    std::unordered_map<EntityId, std::size_t> indices_;
};

}
