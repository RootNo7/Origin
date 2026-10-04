#include "engine/world/world.hpp"
#include <random>
#include <cmath>
#include <algorithm>
namespace origin{void World::generate_vearth(std::uint32_t seed){std::mt19937 r(seed);std::uniform_real_distribution<double> n(-1.2,1.2);for(std::size_t x=0;x<w_;++x){double g=std::clamp(7.0+std::sin(x*.075)*2.5+std::sin(x*.23)*.8+n(r),2.0,(double)h_-2);c_[x].ground_height_m=g;c_[x].temperature_k=286+6*std::sin((double)x/w_*3.14159);c_[x].water_depth_m=g<sea_?sea_-g:0;c_[x].humidity=c_[x].water_depth_m>.0?.85:.45;}}double World::ground_height(double x)const{double q=std::clamp(x,0.0,(double)w_-1);auto a=(std::size_t)std::floor(q);auto b=std::min(a+1,w_-1);double t=q-a;return c_[a].ground_height_m*(1-t)+c_[b].ground_height_m*t;}}
