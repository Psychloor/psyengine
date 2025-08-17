//
// Created by blomq on 2025-08-17.
//

#ifndef PSYENGINE_VECTOR2_HPP // NOLINT(*-redundant-preprocessor)
#define PSYENGINE_VECTOR2_HPP

namespace psyengine::math
{
    template <typename T> requires std::is_arithmetic_v<T>
    struct Vector2
    {
        T x, y;

        Vector2() = default;

        explicit Vector2(T x, T y);

        explicit constexpr Vector2(T value);

        [[nodiscard]] T length() const;
        [[nodiscard]] T lengthSquared() const;

        [[nodiscard]] Vector2 normalized() const;
        Vector2& normalize();

        [[nodiscard]] T dot(const Vector2& other) const;
        [[nodiscard]] Vector2 cross(const Vector2& other) const;

        [[nodiscard]] T distance(const Vector2& other) const;
        [[nodiscard]] T distanceSquared(const Vector2& other) const;

        [[nodiscard]] double angle(const Vector2& other) const;
        [[nodiscard]] double angle(const Vector2& other, const Vector2& reference) const;

        [[nodiscard]] Vector2 perpendicular() const;
        [[nodiscard]] Vector2 perpendicular(const Vector2& reference) const;

        [[nodiscard]] Vector2 reflect(const Vector2& normal) const;


        Vector2(const Vector2&) = default;
        Vector2(Vector2&&) = default;
        ~Vector2() = default;

        Vector2& operator=(const Vector2&) = default;
        Vector2& operator=(Vector2&&) = default;

        bool operator==(const Vector2& other) const;
        bool operator!=(const Vector2& other) const;

        consteval Vector2 operator*(T scalar);
        consteval Vector2 operator*(Vector2 right);
        consteval Vector2 operator/(T scalar);
        consteval Vector2 operator/(Vector2 other);
        consteval Vector2 operator+(T scalar);
        consteval Vector2 operator+(Vector2 other);
        consteval Vector2 operator-(T scalar);
        consteval Vector2 operator-(Vector2 other);

        consteval Vector2& operator*=(T scalar);
        consteval Vector2& operator*=(Vector2& other);
        consteval Vector2& operator/=(T scalar);
        consteval Vector2& operator/=(Vector2& other);
        consteval Vector2& operator+=(T scalar);
        consteval Vector2& operator+=(Vector2& other);
        consteval Vector2& operator-=(T scalar);
        consteval Vector2& operator-=(Vector2& other);

        static constinit Vector2 zero = {Vector2(static_cast<T>(0))};
        static constinit Vector2 one = {Vector2(static_cast<T>(1))};
    };

    using Vector2F = Vector2<float>;
    using Vector2I = Vector2<int>;
    using Vector2U = Vector2<unsigned>;
    using Vector2D = Vector2<double>;

}

#include "psyengine/math/vector2.ipp"

#endif //PSYENGINE_VECTOR2_HPP
