//
// Created by blomq on 2025-08-17.
//

#ifndef PSYENGINE_VECTOR_HPP
#define PSYENGINE_VECTOR_HPP

#include <array>
#include <cmath>
#include <initializer_list>

namespace psyengine::math
{
    template <typename T, size_t N>
    struct Vector;

    // Enable ADL for begin/end
    template <typename T, size_t N>
    auto begin(Vector<T, N>& v) noexcept
    {
        return v.begin();
    }

    template <typename T, size_t N>
    auto end(Vector<T, N>& v) noexcept
    {
        return v.end();
    }

    template <typename T, size_t N>
    auto begin(const Vector<T, N>& v) noexcept
    {
        return v.begin();
    }

    template <typename T, size_t N>
    auto end(const Vector<T, N>& v) noexcept
    {
        return v.end();
    }

    template <typename T, size_t N>
    struct Vector
    {
        std::array<T, N> data;

        // Default constructor - zero initialize
        Vector() :
            data{} {}

        // Constructor from the initializer list
        Vector(std::initializer_list<T> list)
        {
            size_t i = 0;
            for (auto it = list.begin(); it != list.end() && i < N; ++it, ++i)
            {
                data[i] = *it;
            }
            // Fill the remaining with zeros if the list is shorter
            for (; i < N; ++i)
            {
                data[i] = T{};
            }
        }

        // Single value constructor (fills all components)
        explicit Vector(T value)
        {
            data.fill(value);
        }

        // Copy constructor
        Vector(const Vector& other) = default;
        Vector& operator=(const Vector& other) = default;

        // Element access
        T& operator[](size_t index)
        {
            return data[index];
        }

        const T& operator[](size_t index) const
        {
            return data[index];
        }

        T& at(size_t index)
        {
            return data.at(index);
        }

        const T& at(size_t index) const
        {
            return data.at(index);
        }

        // Iterators for range-based loops
        auto begin() noexcept
        {
            return data.begin();
        }

        auto end() noexcept
        {
            return data.end();
        }

        auto begin() const noexcept
        {
            return data.begin();
        }

        auto end() const noexcept
        {
            return data.end();
        }

        auto cbegin() const noexcept
        {
            return data.cbegin();
        }

        auto cend() const noexcept
        {
            return data.cend();
        }

        static constexpr size_t size()
        {
            return N;
        }

        // Arithmetic operators
        Vector operator+(const Vector& other) const
        {
            Vector result;
            for (size_t i = 0; i < N; ++i)
            {
                result[i] = data[i] + other[i];
            }
            return result;
        }

        Vector operator-(const Vector& other) const
        {
            Vector result;
            for (size_t i = 0; i < N; ++i)
            {
                result[i] = data[i] - other[i];
            }
            return result;
        }

        Vector operator*(T scalar) const
        {
            Vector result;
            for (size_t i = 0; i < N; ++i)
            {
                result[i] = data[i] * scalar;
            }
            return result;
        }

        Vector operator/(T scalar) const
        {
            Vector result;
            for (size_t i = 0; i < N; ++i)
            {
                result[i] = data[i] / scalar;
            }
            return result;
        }

        // Unary operators
        Vector operator-() const
        {
            Vector result;
            for (size_t i = 0; i < N; ++i)
            {
                result[i] = -data[i];
            }
            return result;
        }

        // Compound assignment operators
        Vector& operator+=(const Vector& other)
        {
            for (size_t i = 0; i < N; ++i)
            {
                data[i] += other[i];
            }
            return *this;
        }

        Vector& operator-=(const Vector& other)
        {
            for (size_t i = 0; i < N; ++i)
            {
                data[i] -= other[i];
            }
            return *this;
        }

        Vector& operator*=(T scalar)
        {
            for (size_t i = 0; i < N; ++i)
            {
                data[i] *= scalar;
            }
            return *this;
        }

        Vector& operator/=(T scalar)
        {
            for (size_t i = 0; i < N; ++i)
            {
                data[i] /= scalar;
            }
            return *this;
        }

        // Comparison operators
        bool operator==(const Vector& other) const
        {
            for (size_t i = 0; i < N; ++i)
            {
                if (data[i] != other[i])
                    return false;
            }
            return true;
        }

        bool operator!=(const Vector& other) const
        {
            return !(*this == other);
        }

        // Vector operations
        T dot(const Vector& other) const
        {
            T result = T{};
            for (size_t i = 0; i < N; ++i)
            {
                result += data[i] * other[i];
            }
            return result;
        }

        T lengthSquared() const
        {
            return dot(*this);
        }

        T length() const
        {
            return std::sqrt(lengthSquared());
        }

        Vector normalized() const
        {
            T len = length();
            if (len == T{})
                return *this; // Avoid division by zero
            return *this / len;
        }

        Vector& normalize()
        {
            *this = normalized();
            return *this;
        }

        // Distance functions
        T distanceSquaredTo(const Vector& other) const
        {
            return (*this - other).lengthSquared();
        }

        T distanceTo(const Vector& other) const
        {
            return (*this - other).length();
        }

        Vector lerp(const Vector& other, float t) const
        {
            return (*this * (1.0f - t)) + (other * t);
        }
    };

    // Non-member operators for scalar multiplication (scalar * vector)
    template <typename T, size_t N>
    Vector<T, N> operator*(T scalar, const Vector<T, N>& vec)
    {
        return vec * scalar;
    }

    // Specialization constructors for common dimensions
    // 2D Vector specialization
    template <typename T>
    struct Vector<T, 2>
    {
        std::array<T, 2> data;

        Vector() :
            data{} {}

        Vector(T x, T y) :
            data{x, y} {}

        Vector(std::initializer_list<T> list) :
            Vector()
        {
            size_t i = 0;
            for (auto it = list.begin(); it != list.end() && i < 2; ++it, ++i)
            {
                data[i] = *it;
            }
        }

        explicit Vector(T value) :
            data{value, value} {}

        // Accessors with meaningful names
        T& x()
        {
            return data[0];
        }

        T& y()
        {
            return data[1];
        }

        const T& x() const
        {
            return data[0];
        }

        const T& y() const
        {
            return data[1];
        }

        // Include all the same operators and methods as the general template
        // (I'll include the key ones here, but you'd copy all from above)
        T& operator[](size_t index)
        {
            return data[index];
        }

        const T& operator[](size_t index) const
        {
            return data[index];
        }

        Vector operator+(const Vector& other) const
        {
            return Vector(data[0] + other[0], data[1] + other[1]);
        }

        Vector operator-(const Vector& other) const
        {
            return Vector(data[0] - other[0], data[1] - other[1]);
        }

        Vector operator*(T scalar) const
        {
            return Vector(data[0] * scalar, data[1] * scalar);
        }

        T dot(const Vector& other) const
        {
            return data[0] * other[0] + data[1] * other[1];
        }

        T length() const
        {
            return std::sqrt(data[0] * data[0] + data[1] * data[1]);
        }

        // 2D specific operations
        T cross(const Vector& other) const
        {
            return data[0] * other[1] - data[1] * other[0];
        }

        Vector perpendicular() const
        {
            return Vector(-data[1], data[0]);
        }

        Vector rotated(T angle) const
        {
            return Vector(
                x() * std::cos(angle) - y() * std::sin(angle),
                x() * std::sin(angle) + y() * std::cos(angle)
                );
        }

        Vector rotated(T angle, T centerX, T centerY) const
        {
            return rotated(angle) - Vector(centerX, centerY);
        }

        Vector rotated(T angle, const Vector& center) const
        {
            return rotated(angle, center.x(), center.y());
        }
    };

    // 3D Vector specialization
    template <typename T>
    struct Vector<T, 3>
    {
        std::array<T, 3> data;

        Vector() :
            data{} {}

        Vector(T x, T y, T z) :
            data{x, y, z} {}

        Vector(std::initializer_list<T> list) :
            Vector()
        {
            size_t i = 0;
            for (auto it = list.begin(); it != list.end() && i < 3; ++it, ++i)
            {
                data[i] = *it;
            }
        }

        explicit Vector(T value) :
            data{value, value, value} {}

        // Accessors
        T& x()
        {
            return data[0];
        }

        T& y()
        {
            return data[1];
        }

        T& z()
        {
            return data[2];
        }

        const T& x() const
        {
            return data[0];
        }

        const T& y() const
        {
            return data[1];
        }

        const T& z() const
        {
            return data[2];
        }

        // Basic operations (similar to 2D)
        T& operator[](size_t index)
        {
            return data[index];
        }

        const T& operator[](size_t index) const
        {
            return data[index];
        }

        Vector operator+(const Vector& other) const
        {
            return Vector(data[0] + other[0], data[1] + other[1], data[2] + other[2]);
        }

        Vector operator-(const Vector& other) const
        {
            return Vector(data[0] - other[0], data[1] - other[1], data[2] - other[2]);
        }

        Vector operator*(T scalar) const
        {
            return Vector(data[0] * scalar, data[1] * scalar, data[2] * scalar);
        }

        T dot(const Vector& other) const
        {
            return data[0] * other[0] + data[1] * other[1] + data[2] * other[2];
        }

        T length() const
        {
            return std::sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
        }

        // 3D-specific operations
        Vector cross(const Vector& other) const
        {
            return Vector(
                data[1] * other[2] - data[2] * other[1],
                data[2] * other[0] - data[0] * other[2],
                data[0] * other[1] - data[1] * other[0]
                );
        }

    };

    template <typename T>
    struct Vector<T, 4>
    {
        std::array<T, 4> data;

        Vector() :
            data{} {}
        Vector(T x, T y, T z, T w) : data{x, y, z, w} {}

        Vector(std::initializer_list<T> list)
        {
            size_t i = 0;
            for (auto it = list.begin(); it != list.end() && i < 4; ++it, ++i)
            {
                data[i] = *it;
            }
        }

        explicit Vector(T value) : data{value, value, value, value} {}


    };

    // Common type aliases
    using Vec2F = Vector<float, 2>;
    using Vec3F = Vector<float, 3>;
    using Vec4F = Vector<float, 4>;
    using Vec2I = Vector<int, 2>;
    using Vec3I = Vector<int, 3>;
    using Vec4I = Vector<int, 4>;
    using Vec2D = Vector<double, 2>;
    using Vec3D = Vector<double, 3>;
    using Vec4D = Vector<double, 4>;
}



#endif //PSYENGINE_VECTOR_HPP
