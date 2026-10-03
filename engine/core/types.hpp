#pragma once

#include <cmath>
#include <cstdint>

namespace origin {

using EntityId = std::uint64_t;

struct Vec2 {
    double x{0.0};
    double y{0.0};

    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
};

inline Vec2 operator+(Vec2 lhs, const Vec2& rhs) {
    lhs += rhs;
    return lhs;
}

inline Vec2 operator*(const Vec2& value, double scalar) {
    return {value.x * scalar, value.y * scalar};
}

inline double length_squared(const Vec2& value) {
    return value.x * value.x + value.y * value.y;
}

inline bool nearly_equal(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) <= epsilon;
}

}
