#pragma once
#include "engine/entities/entity.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
namespace origin{class EntityRegistry{EntityId next_=1;std::vector<Entity> e_;std::unordered_map<EntityId,std::size_t> ix_;public:EntityId create(std::string,Material,const RigidBody&);bool insert_restored(const Entity&);std::size_t size()const{return e_.size();}auto& all(){return e_;}const auto& all()const{return e_;}};}
