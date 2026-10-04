#pragma once
#include "engine/world/world.hpp"
namespace origin{class Environment{double sun_=1,temp_=288.15;public:void reset(){sun_=1;temp_=288.15;}void step(World&,double,double);double sunlight()const{return sun_;}double global_temperature_k()const{return temp_;}};}
