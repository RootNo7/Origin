#pragma once
#include <cmath>
#include <cstdint>

namespace origin {
using EntityId = std::uint64_t;

struct Vec3 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
};

inline Vec3 operator+(Vec3 a, const Vec3& b) {
    a += b;
    return a;
}

inline Vec3 operator*(Vec3 a, double scale) {
    a.x *= scale;
    a.y *= scale;
    a.z *= scale;
    return a;
}

inline bool nearly_equal(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) <= epsilon;
}
}
