#pragma once
#include <cmath>
#include <cstdint>
namespace origin { using EntityId=std::uint64_t; struct Vec2{double x=0,y=0; Vec2& operator+=(const Vec2&o){x+=o.x;y+=o.y;return *this;}}; inline Vec2 operator*(Vec2 a,double s){return {a.x*s,a.y*s};} inline bool nearly_equal(double a,double b,double e=1e-9){return std::abs(a-b)<=e;} }
