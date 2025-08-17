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
        T x;
        T y;

        Vector2() = default;
        explicit Vector2(T x, T y);
        explicit constexpr Vector2(T value);
        ~Vector2() = default;

        /**
         * @brief Normalizes the given vector, scaling it to unit length.
         *
         * This function adjusts the magnitude of the vector to 1 while preserving
         * its direction. If the vector's magnitude is zero, normalization cannot
         * be performed. Users should handle or verify the possibility of a zero
         * vector before calling this function.
         */
        Vector2& normalize();

        /**
         * @brief Computes and returns the length or magnitude.
         * @return The length as a double precision value.
         */
        [[nodiscard]] T length() const;
        /**
         * @brief Computes and returns the squared length (magnitude) of the vector.
         *
         * This method calculates the squared Euclidean length of the 2D vector,
         * which is equivalent to the sum of the squares of its components. The
         * squared length is often used in computations where the actual length
         * is not required, as it avoids the overhead of a square root operation.
         *
         * @tparam T The arithmetic type of the vector components.
         * @return T The squared length of the vector.
         */
        [[nodiscard]] T lengthSquared() const;
        /**
         * @brief Computes and returns the normalized vector.
         *
         * Calculates a unit vector in the same direction as the original vector.
         * This method assumes the vector is not zero-length.
         *
         * @return The normalized vector.
         */
        [[nodiscard]] Vector2 normalized() const;
        /**
         * Computes the dot product of this vector with another vector.
         *
         * The dot product is calculated as the sum of the products of the
         * corresponding components of the two vectors.
         *
         * @tparam T The type of the vector components. Must be an arithmetic type.
         * @param other The other vector to compute the dot product with.
         * @return The resulting dot product as a value of type T.
         */
        [[nodiscard]] T dot(const Vector2& other) const;
        /**
         * Computes the 2D cross-product of this vector with another vector.
         * In 2D, the cross-product is a scalar and represents the magnitude
         * of the perpendicular component.
         *
         * @tparam T The data type of the vector components, which must satisfy
         *           std::is_arithmetic.
         *
         * @param other The vector to compute the cross product with.
         *
         * @return A new Vector2 object where the x-component represents the
         *         scalar result of the 2D cross product operation.
         */
        [[nodiscard]] Vector2 cross(const Vector2& other) const;
        /**
         * @brief Calculates and returns the distance.
         */
        [[nodiscard]] T distance(const Vector2& other) const;
        /**
         * Calculates the squared distance between this vector and another vector.
         *
         * @tparam T The numeric type of the vector components. Must satisfy the requirement of being an arithmetic type.
         * @param other The vector to calculate the squared distance to.
         * @return The squared distance between this vector and the specified vector.
         *
         * This function avoids computing the square root, making it more efficient than calculating the actual distance
         * when only a comparison of distances is required.
         */
        [[nodiscard]] T distanceSquared(const Vector2& other) const;
        /**
         * @brief Calculates the angle between the current vector and another vector.
         *
         * This function computes the angle between the current 2-dimensional vector
         * and another 2-dimensional vector using the arctangent of their relative
         * coordinates.
         *
         * @tparam T The type of the vector components, which must be arithmetic.
         * @param other The other vector to calculate the angle with.
         * @return The angle in radians between the two vectors.
         */
        [[nodiscard]] double angle(const Vector2& other) const;
        /**
         * @brief Computes the angle difference between two vectors relative to a reference vector.
         *
         * The method calculates the signed directional angle difference from the reference vector
         * to the other vector in the context of the calling vector. It uses the arctangent function
         * to determine the angular difference.
         *
         * @tparam T The type of the components of the vector, restricted to arithmetic types.
         *
         * @param other The vector to compute the angle difference to.
         * @param reference The vector to use as the reference for the angle calculation.
         *
         * @return The signed angle difference in radians between the two vectors, calculated
         *         relative to the reference vector.
         */
        [[nodiscard]] double angle(const Vector2& other, const Vector2& reference) const;
        /**
         * @brief Computes a vector that is perpendicular to the current vector.
         *
         * The method returns a new vector that is obtained by rotating the current
         * vector 90 degrees in a counterclockwise direction. The x-coordinate
         * of the perpendicular vector is the negation of the y-coordinate of the
         * current vector, and the y-coordinate of the perpendicular vector is the
         * x-coordinate of the current vector.
         *
         * @tparam T The type of the vector components, which must be an arithmetic type (e.g., int, float, double).
         * @return Vector2<T> A new vector that is perpendicular to the current vector.
         */
        [[nodiscard]] Vector2 perpendicular() const;
        /**
         * Calculates a vector that is perpendicular to this vector relative to the given reference vector.
         *
         * @tparam T The numeric type of the vector components. Must satisfy `std::is_arithmetic`.
         * @param reference Another vector to be used as a reference in the calculation.
         * @return A new vector that is perpendicular to this vector relative to the reference vector.
         */
        [[nodiscard]] Vector2 perpendicular(const Vector2& reference) const;
        /**
         * Reflects the vector around a given normal vector.
         *
         * This method calculates the reflection of the current vector
         * with respect to the specified normal vector. The reflected
         * vector is computed using the formula:
         *
         * result = this - 2 * (dot product of this and normal) * normal
         *
         * @tparam T The numeric type of the vector's components (e.g., float, double, int).
         *           Must satisfy `std::is_arithmetic`.
         *
         * @param normal The normal vector used for reflection. It is assumed to be normalized.
         *
         * @return A new vector that is the reflection of the current vector.
         */
        [[nodiscard]] Vector2 reflect(const Vector2& normal) const;
        /**
         * Performs a linear interpolation between the current vector and another vector.
         *
         * The function computes a vector that lies t fraction of the way between this
         * vector and the specified vector, 'other'. The interpolation is based on the
         * scalar value 't', which should be in the range [0.0, 1.0]. When t is 0, the
         * result is this vector; when t is 1, the result is the 'other' vector.
         *
         * @tparam T The type of the vector components. Must be an arithmetic type.
         * @param other The target vector to interpolate toward.
         * @param t The interpolation factor, ranging from 0.0 to 1.0.
         * @return A Vector2 object representing the interpolated result.
         */
        [[nodiscard]] Vector2 lerp(const Vector2& other, float t) const;
        /**
         * Calculates a smoothstep interpolation between the current vector and another vector.
         *
         * The smoothstep function performs an interpolation between the current vector and
         * the provided `other` vector based on the time parameter `t`. The function uses
         * a cubic Hermite interpolation, ensuring a smooth transition with zero derivative at both ends.
         *
         * @tparam T The arithmetic type of the vector components.
         * @param other The target vector to interpolate towards.
         * @param t The interpolation ratio, typically in the range [0, 1].
         *          Values outside this range can produce results beyond the vectors' bounds.
         * @return A new vector resulting from the smoothstep interpolation.
         */
        [[nodiscard]] Vector2 smoothstep(const Vector2& other, float t) const;
        /**
         * @brief Computes a stepped interpolation between the current vector
         *        and another vector.
         *
         * This method returns a vector that is the result of a stepped interpolation
         * towards the given `other` vector. The step is determined by the value of the
         * parameter `t`. If `t` is greater than 0, the function interpolates fully
         * towards the `other` vector; otherwise, it returns the current vector.
         *
         * @tparam T The type of the scalar components of the vector. Must satisfy
         *         the `std::is_arithmetic` constraint.
         * @param other The target vector to step towards.
         * @param t A scalar threshold to determine the interpolation. If `t > 0`,
         *        full interpolation is applied; otherwise, no interpolation occurs.
         * @return A new vector resulting from the stepped interpolation.
         */
        [[nodiscard]] Vector2 step(const Vector2& other, float t) const;
        /**
         * @brief Restricts a given value to lie between a specified minimum and maximum range.
         *
         * This function ensures that the input value does not exceed the specified limits
         * by clamping it to the nearest boundary if it is outside the defined range.
         *
         * @param min The lower boundary of the range.
         * @param max The upper boundary of the range.
         * @return The clamped value within the range [min, max].
         */
        [[nodiscard]] Vector2 clamp(const Vector2& min, const Vector2& max) const;
        /**
         * Clamps the length of the vector to a specified range while maintaining its direction.
         *
         * The method scales the vector so that its length falls within the range defined
         * by the `min` and `max` parameters. If the vector's length is less than `min`,
         * the length is set to `min`. If the vector's length is greater than `max`,
         * the length is set to `max`.
         *
         * @tparam T The type of the vector's components. Must satisfy the arithmetic type constraint.
         * @param min The minimum allowed length of the vector.
         * @param max The maximum allowed length of the vector.
         * @return A new vector with its length clamped to the range [min, max].
         */
        [[nodiscard]] Vector2 clampLength(float min, float max) const;
        /**
         * Clamps the magnitude of the vector to a specified range.
         *
         * This method modifies the magnitude of the vector to ensure it is within
         * the inclusive range defined by the minimum and maximum values provided.
         * If the vector's magnitude is below the minimum, it is adjusted upwards
         * to match the minimum. If the vector's magnitude is above the maximum, it
         * is adjusted downwards to match the maximum. The direction of the vector
         * is preserved during this adjustment.
         *
         * @tparam T The numeric type of the vector components (must be arithmetic).
         * @param min The minimum allowed magnitude.
         * @param max The maximum allowed magnitude.
         * @return A new vector with its magnitude clamped within the specified range.
         */
        [[nodiscard]] Vector2 clampMagnitude(float min, float max) const;
        /**
         * Clamps the angle of the vector within a specified range.
         *
         * The angle is determined by treating the vector as originating at the
         * coordinate (0,0) and measuring its angle relative to the positive X-axis.
         * The clamping operation restricts this angle to the specified range
         * [min, max], in radians.
         *
         * @tparam T The numeric type of the vector components. It must satisfy
         *           the constraint of being an arithmetic type.
         * @param min The minimum angle bound (in radians) for clamping.
         * @param max The maximum angle bound (in radians) for clamping.
         * @return A new Vector2 object with the angle clamped to the specified range.
         */
        [[nodiscard]] Vector2 clampAngle(float min, float max) const;
        /**
         * @brief Rotates a given object or coordinate by a specified angle.
         *
         * This function performs a rotational transformation, rotating the input
         * around a defined axis or point by the provided angle. The rotation
         * direction and behavior depend on the specific implementation and
         * parameters used.
         *
         * @param angle The angle by which the rotation is performed, typically in degrees or radians.
         */
        [[nodiscard]] Vector2 rotate(float angle) const;
        /**
         * @brief Rotates the given object or point by a specified angle or axis.
         *
         * This function performs a rotation transformation, which can either be
         * a 2D or 3D rotation, depending on the context and implementation.
         * The exact behavior and parameters required may vary based on the
         * rotation logic provided.
         */
        [[nodiscard]] Vector2 rotate(float angle, const Vector2& reference) const;
        /**
         * Rotates the current vector around a given point by a specified angle.
         *
         * @tparam T The numeric type of the vector components (must be arithmetic).
         * @param point A vector representing the point around which the rotation is performed.
         * @param angle The angle in radians by which the vector is rotated.
         * @return A new vector that is the result of rotating the current vector around the specified point.
         */
        [[nodiscard]] Vector2 rotateAround(const Vector2& point, float angle) const;
        /**
         * Rotates this vector around a given point by a specified angle, using a reference direction.
         *
         * @tparam T The type of the vector components, requires arithmetic types.
         * @param point The point around which the vector will be rotated.
         * @param angle The angle in radians by which the vector will be rotated.
         * @param reference The reference direction used during the rotation.
         * @return A new vector that is the result of this vector being rotated around the given point by the specified angle.
         */
        [[nodiscard]] Vector2 rotateAround(const Vector2& point, float angle, const Vector2& reference) const;
        /**
         * @brief Calculates the absolute value of a given number.
         *
         * This method returns the non-negative value of the input,
         * regardless of whether the input is positive or negative.
         *
         * @return The absolute value of the input number.
         */
        [[nodiscard]] Vector2 abs() const;
        /**
         * @brief Rounds the given value to the nearest integer.
         *
         * The function uses standard rounding rules. If the decimal part of the
         * value is 0.5 or greater, the value is rounded up. Otherwise, it is rounded down.
         *
         * @return The rounded integer value.
         */
        [[nodiscard]] Vector2 round() const;
        /**
         * @brief Computes and returns the sign of a number.
         *
         * This function evaluates whether the input number is positive, negative, or zero
         * and returns an integer indicating the sign.
         *
         * @return An integer that is -1 for negative numbers, 0 for zero, and 1 for positive numbers.
         */
        [[nodiscard]] Vector2 sign() const;

        Vector2(const Vector2&) = default;
        Vector2(Vector2&&) = default;
        Vector2& operator=(const Vector2&) = default;
        Vector2& operator=(Vector2&&) = default;

        bool operator==(const Vector2& other) const;
        bool operator!=(const Vector2& other) const;

        constexpr Vector2 operator*(T scalar);
        constexpr Vector2 operator*(const Vector2& other);
        constexpr Vector2 operator/(T scalar);
        constexpr Vector2 operator/(const Vector2& other);
        constexpr Vector2 operator+(T scalar);
        constexpr Vector2 operator+(const Vector2& other);
        constexpr Vector2 operator-(T scalar);
        constexpr Vector2 operator-(const Vector2& other);

        constexpr Vector2& operator*=(T scalar);
        constexpr Vector2& operator*=(const Vector2& other);
        constexpr Vector2& operator/=(T scalar);
        constexpr Vector2& operator/=(const Vector2& other);
        constexpr Vector2& operator+=(T scalar);
        constexpr Vector2& operator+=(const Vector2& other);
        constexpr Vector2& operator-=(T scalar);
        constexpr Vector2& operator-=(const Vector2& other);

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
