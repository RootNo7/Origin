#pragma once

#include <cstdint>
#include <array>

namespace origin::physics {

/**
 * @brief 2D vector for physics calculations.
 * 
 * Uses double precision for numerical stability in long-running simulations.
 */
struct Vec2 {
    double x = 0.0;
    double y = 0.0;
    
    constexpr Vec2() noexcept = default;
    constexpr Vec2(double x_, double y_) noexcept : x(x_), y(y_) {}
    
    [[nodiscard]] constexpr Vec2 operator+(const Vec2& other) const noexcept {
        return Vec2{x + other.x, y + other.y};
    }
    
    [[nodiscard]] constexpr Vec2 operator-(const Vec2& other) const noexcept {
        return Vec2{x - other.x, y - other.y};
    }
    
    [[nodiscard]] constexpr Vec2 operator*(double scalar) const noexcept {
        return Vec2{x * scalar, y * scalar};
    }
    
    [[nodiscard]] constexpr Vec2 operator/(double scalar) const noexcept {
        return Vec2{x / scalar, y / scalar};
    }
    
    Vec2& operator+=(const Vec2& other) noexcept {
        x += other.x; y += other.y; return *this;
    }
    
    Vec2& operator-=(const Vec2& other) noexcept {
        x -= other.x; y -= other.y; return *this;
    }
    
    Vec2& operator*=(double scalar) noexcept {
        x *= scalar; y *= scalar; return *this;
    }
    
    Vec2& operator/=(double scalar) noexcept {
        x /= scalar; y /= scalar; return *this;
    }
    
    [[nodiscard]] constexpr double dot(const Vec2& other) const noexcept {
        return x * other.x + y * other.y;
    }
    
    [[nodiscard]] constexpr double cross(const Vec2& other) const noexcept {
        return x * other.y - y * other.x;
    }
    
    [[nodiscard]] constexpr double length_squared() const noexcept {
        return x * x + y * y;
    }
    
    [[nodiscard]] double length() const noexcept {
        return std::sqrt(length_squared());
    }
    
    [[nodiscard]] Vec2 normalized() const noexcept {
        double len = length();
        return len > 0 ? *this / len : Vec2{0, 0};
    }
    
    [[nodiscard]] constexpr bool is_zero() const noexcept {
        return x == 0.0 && y == 0.0;
    }
};

/**
 * @brief 2D transform (position + rotation).
 */
struct Transform2D {
    Vec2 position{0, 0};
    double rotation = 0.0;  // Radians
    
    [[nodiscard]] Vec2 transform_point(const Vec2& local) const noexcept {
        double cos_r = std::cos(rotation);
        double sin_r = std::sin(rotation);
        return Vec2{
            position.x + local.x * cos_r - local.y * sin_r,
            position.y + local.x * sin_r + local.y * cos_r
        };
    }
    
    [[nodiscard]] Vec2 transform_vector(const Vec2& local) const noexcept {
        double cos_r = std::cos(rotation);
        double sin_r = std::sin(rotation);
        return Vec2{
            local.x * cos_r - local.y * sin_r,
            local.x * sin_r + local.y * cos_r
        };
    }
};

/**
 * @brief Axis-aligned bounding box.
 */
struct AABB {
    Vec2 min{0, 0};
    Vec2 max{0, 0};
    
    [[nodiscard]] constexpr Vec2 center() const noexcept {
        return (min + max) * 0.5;
    }
    
    [[nodiscard]] constexpr Vec2 size() const noexcept {
        return max - min;
    }
    
    [[nodiscard]] constexpr bool contains(const Vec2& point) const noexcept {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y;
    }
    
    [[nodiscard]] constexpr bool overlaps(const AABB& other) const noexcept {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y;
    }
    
    void expand(const Vec2& point) noexcept {
        min.x = std::min(min.x, point.x);
        min.y = std::min(min.y, point.y);
        max.x = std::max(max.x, point.x);
        max.y = std::max(max.y, point.y);
    }
    
    void expand(const AABB& other) noexcept {
        min.x = std::min(min.x, other.min.x);
        min.y = std::min(min.y, other.min.y);
        max.x = std::max(max.x, other.max.x);
        max.y = std::max(max.y, other.max.y);
    }
};

} // namespace origin::physics