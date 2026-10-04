#include "engine/environment/environment.hpp"
#include <cmath>
#include <algorithm>
namespace origin{void Environment::step(World&w,double t,double dt){double p=std::fmod(t,86400.)/86400.;sun_=std::max(0.,std::sin(p*6.283185-1.570796));temp_=286+3*sun_;for(std::size_t x=0;x<w.width();++x){auto&c=w.columns()[x];double target=temp_+4*std::sin(x*.04);c.temperature_k+=(target-c.temperature_k)*std::clamp(dt/120.,0.,1.);}}}
