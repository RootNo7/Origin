#pragma once
#include "engine/core/types.hpp"
#include "engine/world/world.hpp"
#include <string>
namespace origin{struct RigidBody{Vec2 position{},velocity{};double mass_kg=1,radius_m=.35,restitution=.2;bool dynamic=true;};struct Entity{EntityId id=0;std::string name;RigidBody body;Material material=Material::Rock;bool alive=true;};}
