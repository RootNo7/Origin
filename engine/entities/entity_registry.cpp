#include "engine/entities/entity_registry.hpp"

#include <algorithm>

namespace origin {

EntityId EntityRegistry::create(std::string name, Material material, const RigidBody& body) {
    const EntityId id = next_id_++;
    entities_.push_back(Entity{id, std::move(name), body, material, true});
    indices_[id] = entities_.size() - 1;
    return id;
}

Entity* EntityRegistry::find(EntityId id) {
    const auto it = indices_.find(id);
    if (it == indices_.end()) return nullptr;
    return &entities_[it->second];
}

const Entity* EntityRegistry::find(EntityId id) const {
    const auto it = indices_.find(id);
    if (it == indices_.end()) return nullptr;
    return &entities_[it->second];
}

}

bool origin::EntityRegistry::insert_restored(const Entity& entity) {
    if (entity.id == 0 || indices_.contains(entity.id)) {
        return false;
    }
    entities_.push_back(entity);
    indices_[entity.id] = entities_.size() - 1;
    next_id_ = std::max(next_id_, entity.id + 1);
    return true;
}
