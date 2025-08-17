//
// Created by blomq on 2025-08-17.
//

#ifndef PSYENGINE_VECTOR2_IPP
#define PSYENGINE_VECTOR2_IPP

#include "vector2.hpp"

#include <cmath>
#include <limits>

namespace psyengine::math
{

    template <typename T> requires std::is_arithmetic_v<T>
    Vector2<T>::Vector2(const T x, const T y): // NOLINT(*-easily-swappable-parameters)
        x(x), y(y)
    {}

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
    Vector2<T>& Vector2<T>::normalize()
    {
        *this = normalized();
        return *this;
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
    bool Vector2<T>::operator==(const Vector2& other) const
    {
        return std::abs(x - other.x) < std::numeric_limits<T>::epsilon() && std::abs(y - other.y) < std::numeric_limits<
            T>::epsilon();
    }

    template <typename T> requires std::is_arithmetic_v<T>
    bool Vector2<T>::operator!=(const Vector2& other) const
    {
        return std::abs(x - other.x) >= std::numeric_limits<T>::epsilon() || std::abs(y - other.y) >= std::numeric_limits<
            T>::epsilon();
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Vector2<T>::Vector2(const T value):
        x(value), y(value)
    {}

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T> Vector2<T>::operator*(T scalar)
    {
        return {x * scalar, y * scalar};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T> Vector2<T>::operator*(Vector2 right)
    {
        return {x * right.x, y * right.y};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T> Vector2<T>::operator/(T scalar)
    {
        return {x / scalar, y / scalar};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T> Vector2<T>::operator/(Vector2 other)
    {
        return {x / other.x, y / other.y};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T> Vector2<T>::operator+(T scalar)
    {
        return {x + scalar, y + scalar};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T> Vector2<T>::operator+(Vector2 other)
    {
        return {x + other.x, y + other.y};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T> Vector2<T>::operator-(T scalar)
    {
        return {x - scalar, y - scalar};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T> Vector2<T>::operator-(Vector2 other)
    {
        return {x - other.x, y - other.y};
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T>& Vector2<T>::operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T>& Vector2<T>::operator*=(Vector2& other)
    {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T>& Vector2<T>::operator/=(T scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T>& Vector2<T>::operator/=(Vector2& other)
    {
        x /= other.x;
        y /= other.y;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T>& Vector2<T>::operator+=(T scalar)
    {
        x += scalar;
        y += scalar;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T>& Vector2<T>::operator+=(Vector2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T>& Vector2<T>::operator-=(T scalar)
    {
        x -= scalar;
        y -= scalar;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    consteval Vector2<T>& Vector2<T>::operator-=(Vector2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
}

#endif //PSYENGINE_VECTOR2_IPP