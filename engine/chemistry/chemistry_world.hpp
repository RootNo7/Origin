#pragma once
#include "engine/world/world.hpp"
namespace origin{enum class WaterPhase{None,Ice,Liquid,Vapor};inline WaterPhase water_phase(double k){return k<=273.15?WaterPhase::Ice:(k>=373.15?WaterPhase::Vapor:WaterPhase::Liquid);}class ChemistryWorld{public:void update(World&)const;};}
