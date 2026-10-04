#pragma once
#include "engine/time/simulation_clock.hpp"
#include "engine/world/world.hpp"
#include "engine/entities/entity_registry.hpp"
#include "engine/physics/physics_world.hpp"
#include "engine/chemistry/chemistry_world.hpp"
#include "engine/environment/environment.hpp"
#include "engine/events/event_bus.hpp"
namespace origin{class Simulation{SimulationClock clock_;World world_;EntityRegistry entities_;PhysicsWorld physics_;ChemistryWorld chemistry_;Environment environment_;EventBus events_;public:Simulation(std::size_t w,std::size_t h):world_(w,h){}void initialize(std::uint32_t);void step();auto&clock(){return clock_;}const auto&clock()const{return clock_;}auto&world(){return world_;}const auto&world()const{return world_;}auto&entities(){return entities_;}const auto&entities()const{return entities_;}const auto&environment()const{return environment_;}auto&events(){return events_;}};}
