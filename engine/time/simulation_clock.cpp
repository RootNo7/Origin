#include "engine/time/simulation_clock.hpp"
#include <algorithm>
namespace origin{void SimulationClock::reset(double d){tick_=0;sec_=0;dt_=d>0?d:1.0/30.0;speed_=1;paused_=false;}void SimulationClock::advance(){if(!paused_){sec_+=dt_*speed_;++tick_;}}void SimulationClock::restore(std::uint64_t t,double s,double d,double sp,bool p){tick_=t;sec_=s;dt_=d>0?d:1.0/30.0;speed_=std::max(0.0,sp);paused_=p;}}
