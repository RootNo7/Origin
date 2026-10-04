#include "engine/entities/entity_registry.hpp"

namespace origin {
EntityId EntityRegistry::create(std::string name, Material material, const RigidBody& body) {
    const EntityId id = next_++;
    entities_.push_back({id, std::move(name), body, material, true, {}});
    index_[id] = entities_.size() - 1;
    return id;
}

bool EntityRegistry::insert_restored(const Entity& entity) {
    if (!entity.id || index_.contains(entity.id)) return false;
    entities_.push_back(entity);
    index_[entity.id] = entities_.size() - 1;
    next_ = std::max(next_, entity.id + 1);
    return true;
}

Entity* EntityRegistry::find(EntityId id) {
    const auto it = index_.find(id);
    return it == index_.end() ? nullptr : &entities_[it->second];
}

const Entity* EntityRegistry::find(EntityId id) const {
    const auto it = index_.find(id);
    return it == index_.end() ? nullptr : &entities_[it->second];
}

void EntityRegistry::clear() {
    entities_.clear();
    index_.clear();
    next_ = 1;
}
}
