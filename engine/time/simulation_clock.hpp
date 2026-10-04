#pragma once
#include <cstdint>
namespace origin {class SimulationClock{std::uint64_t tick_=0;double sec_=0,dt_=1.0/30.0,speed_=1;bool paused_=false;public:void reset(double=1.0/30.0);void advance();void restore(std::uint64_t,double,double,double,bool);std::uint64_t tick()const{return tick_;}double seconds()const{return sec_;}double fixed_dt()const{return dt_;}double speed()const{return speed_;}bool paused()const{return paused_;}};}
