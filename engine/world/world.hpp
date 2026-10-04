#pragma once
#include "engine/core/types.hpp"
#include <vector>
#include <cstdint>
namespace origin{enum class Material{Air,Water,Soil,Rock,Ice};struct Column{double ground_height_m=0,temperature_k=288.15,water_depth_m=0,humidity=.5;};class World{std::size_t w_,h_;double sea_=8;std::vector<Column> c_;public:World(std::size_t w,std::size_t h):w_(w),h_(h),c_(w){}void generate_vearth(std::uint32_t);double ground_height(double)const;std::size_t width()const{return w_;}std::size_t height()const{return h_;}double sea_level_m()const{return sea_;}const std::vector<Column>& columns()const{return c_;}std::vector<Column>& columns(){return c_;}};}
