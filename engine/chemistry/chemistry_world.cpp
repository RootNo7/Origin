#include "engine/chemistry/chemistry_world.hpp"
namespace origin{void ChemistryWorld::update(World&w)const{for(auto&c:w.columns())if(c.water_depth_m>0&&water_phase(c.temperature_k)==WaterPhase::Vapor)c.humidity=1;}}
