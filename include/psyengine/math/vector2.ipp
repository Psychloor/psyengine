//
// Created by blomq on 2025-08-17.
//

#ifndef PSYENGINE_VECTOR2_IPP
#define PSYENGINE_VECTOR2_IPP

#include "vector2.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>

#include "psyengine/debug/assert.hpp"

namespace psyengine::math
{

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T>::Vector2(const T x, const T y) : // NOLINT(*-easily-swappable-parameters)
        x(x), y(y) {}

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T>::Vector2(const T value) :
        x(value), y(value) {}

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T>& Vector2<T>::normalize()
    {
        *this = normalized();
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    T Vector2<T>::length() const
    {
        return std::sqrt(x * x + y * y);
    }

    template <typename T> requires std::is_arithmetic_v<T>
    T Vector2<T>::lengthSquared() const
    {
        return x * x + y * y;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::normalized() const
    {
        return *this / length();
    }

    template <typename T> requires std::is_arithmetic_v<T>
    T Vector2<T>::dot(const Vector2& other) const
    {
        return x * other.x + y * other.y;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::cross(const Vector2& other) const
    {
        return {x * other.y - y * other.x};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    T Vector2<T>::distance(const Vector2& other) const
    {
        return (*this - other).length();
    }

    template <typename T> requires std::is_arithmetic_v<T>
    T Vector2<T>::distanceSquared(const Vector2& other) const
    {
        return (*this - other).lengthSquared();
    }

    template <typename T> requires std::is_arithmetic_v<T>
    double Vector2<T>::angle(const Vector2& other) const
    {
        return std::atan2(static_cast<double>(other.y - y), static_cast<double>(other.x - x));
    }

    template <typename T> requires std::is_arithmetic_v<T>
    double Vector2<T>::angle(const Vector2& other, const Vector2& reference) const
    {
        return std::atan2(static_cast<double>(other.y - y), static_cast<double>(other.x - x)) -
            std::atan2(static_cast<double>(reference.y - y), static_cast<double>(reference.x - x));
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::perpendicular() const
    {
        return {-y, x};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::perpendicular(const Vector2& reference) const
    {
        return {y - reference.y, x - reference.x};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::reflect(const Vector2& normal) const
    {
        return *this - 2 * dot(normal) * normal;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::lerp(const Vector2& other, float t) const
    {
        return *this + (other - *this) * t;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::smoothstep(const Vector2& other, const float t) const
    {
        return lerp(other, t * t * (3 - 2 * t));
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::step(const Vector2& other, const float t) const
    {
        return lerp(other, t > 0 ? 1 : 0);
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::clamp(const Vector2& min, const Vector2& max) const
    {
        return {std::clamp(x, min.x, max.x), std::clamp(y, min.y, max.y)};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::clampLength(float min, float max) const
    {
        return *this * std::clamp(length(), min, max) / length();
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::clampMagnitude(const float min, const float max) const
    {
        return *this * std::clamp(lengthSquared(), min * min, max * max) / length();
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::clampAngle(float min, float max) const
    {
        return *this * std::clamp(angle(Vector2{1, 0}), min, max);
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::rotate(const float angle) const
    {
        return *this * std::cos(angle) - perpendicular() * std::sin(angle);
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::rotate(const float angle, const Vector2& reference) const
    {
        return *this * std::cos(angle) - perpendicular(reference) * std::sin(angle);
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::rotateAround(const Vector2& point, const float angle) const
    {
        return *this - point + rotate(angle, point);
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::rotateAround(const Vector2& point, const float angle, const Vector2& reference) const
    {
        return *this - point + rotate(angle, reference);
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::abs() const
    {
        return {std::abs(x), std::abs(y)};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::round() const
    {
        return {std::round(x), std::round(y)};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T> Vector2<T>::sign() const
    {
        return {std::signbit(x) ? -1 : 1, std::signbit(y) ? -1 : 1};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    bool Vector2<T>::operator==(const Vector2& other) const
    {
        return std::abs(x - other.x) < std::numeric_limits<T>::epsilon() && std::abs(y - other.y) < std::numeric_limits<
            T>::epsilon();
    }

    template <typename T> requires std::is_arithmetic_v<T>
    bool Vector2<T>::operator!=(const Vector2& other) const
    {
        return std::abs(x - other.x) >= std::numeric_limits<T>::epsilon() || std::abs(y - other.y) >=
            std::numeric_limits<
                T>::epsilon();
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T> Vector2<T>::operator*(T scalar)
    {
        return {x * scalar, y * scalar};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T> Vector2<T>::operator*(const Vector2& other)
    {
        return {x * other.x, y * other.y};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T> Vector2<T>::operator/(T scalar)
    {
        if constexpr (std::is_integral_v<T>)
        {
            if (scalar == 0)
            {
                if consteval
                {
                    throw std::exception("Vector2: integer division by zero in constant evaluation");
                }
                PSY_ASSERT(false, "Vector2: integer division by zero");
                return {static_cast<T>(0), static_cast<T>(0)};
            }
        }
        else
        {
#ifndef NDEBUG
            PSY_DEBUG_ASSERT(scalar != static_cast<T>(0), "Vector2: floating division by zero");
#endif
        }

        return {x / scalar, y / scalar};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T> Vector2<T>::operator/(const Vector2& other)
    {
        if constexpr (std::is_integral_v<T>)
        {
            if (other.x == 0 || other.y == 0)
            {
                if consteval
                {
                    throw std::exception("Vector2: integer component-wise division by zero in constant evaluation");
                }
                PSY_ASSERT(false, "Vector2: integer component-wise division by zero");
                return {static_cast<T>(0), static_cast<T>(0)};
            }
        }
        else
        {
#ifndef NDEBUG
            PSY_DEBUG_ASSERT(other.x != static_cast<T>(0) && other.y != static_cast<T>(0),
                             "Vector2: floating component-wise division by zero");
#endif
        }

        return {x / other.x, y / other.y};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T> Vector2<T>::operator+(T scalar)
    {
        return {x + scalar, y + scalar};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T> Vector2<T>::operator+(const Vector2& other)
    {
        return {x + other.x, y + other.y};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T> Vector2<T>::operator-(T scalar)
    {
        return {x - scalar, y - scalar};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T> Vector2<T>::operator-(const Vector2& other)
    {
        return {x - other.x, y - other.y};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T>& Vector2<T>::operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T>& Vector2<T>::operator*=(const Vector2& other)
    {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T>& Vector2<T>::operator/=(T scalar)
    {
        if constexpr (std::is_integral_v<T>)
        {
            if (scalar == 0)
            {
                if consteval
                {
                    throw std::exception("Vector2: integer division by zero in constant evaluation");
                }
                PSY_ASSERT(false, "Vector2: integer division by zero");
                x = static_cast<T>(0);
                y = static_cast<T>(0);
                return *this;
            }
        }
        else
        {
#ifndef NDEBUG
            PSY_DEBUG_ASSERT(scalar != static_cast<T>(0), "Vector2: floating division by zero");
#endif
        }

        x /= scalar;
        y /= scalar;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T>& Vector2<T>::operator/=(const Vector2& other)
    {
        if constexpr (std::is_integral_v<T>)
        {
            if (other.x == 0 || other.y == 0)
            {
                if consteval
                {
                    throw std::exception("Vector2: integer component-wise division by zero in constant evaluation");
                }
                PSY_ASSERT(false, "Vector2: integer component-wise division by zero");
                x = static_cast<T>(0);
                y = static_cast<T>(0);
                return *this;
            }
        }
        else
        {
#ifndef NDEBUG
            PSY_DEBUG_ASSERT(other.x != static_cast<T>(0) && other.y != static_cast<T>(0),
                             "Vector2: floating component-wise division by zero");
#endif
        }

        x /= other.x;
        y /= other.y;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T>& Vector2<T>::operator+=(T scalar)
    {
        x += scalar;
        y += scalar;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T>& Vector2<T>::operator+=(const Vector2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T>& Vector2<T>::operator-=(T scalar)
    {
        x -= scalar;
        y -= scalar;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T>& Vector2<T>::operator-=(const Vector2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
}

#endif //PSYENGINE_VECTOR2_IPP
