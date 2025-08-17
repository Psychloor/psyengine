//
// Created by blomq on 2025-08-17.
//

#ifndef PSYENGINE_MATH_UTILS_HPP
#define PSYENGINE_MATH_UTILS_HPP

#include <cmath>

namespace psyengine::math
{
    static constexpr double PI = 3.14159265358979323846;
    static constexpr double TWO_PI = 2 * PI;
    static constexpr double HALF_PI = PI / 2;
    static constexpr double QUARTER_PI = PI / 4;
    static constexpr double E = 2.71828182845904523536;
    static constexpr double EULER = 0.57721566490153286061;
    static constexpr double ROOT_TWO = 1.41421356237309504880;
    static constexpr double ROOT_THREE = 1.73205080756887729353;
    static constexpr double ROOT_FIVE = 2.23606797749978969641;
    static constexpr double ROOT_SEVEN = 2.64575131106459059057;
    static constexpr double ROOT_TEN = 3.16227766016837933196;
    static constexpr double DEG_TO_RAD = PI / 180.0;

    static constexpr double RadToDegrees(double radians)
    {
        return radians * 180.0 / PI;
    }

    static constexpr double DegreesToRad(double degrees)
    {
        return degrees * PI / 180.0;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    T Clamp(const T& value, const T& min, const T& max)
    {
        return std::max(min, std::min(max, value));
    }

    template <typename T> requires std::is_arithmetic_v<T>
    T Lerp(const T& a, const T& b, const T& t)
    {
        return a + (b - a) * t;
    }


}

#endif //PSYENGINE_MATH_UTILS_HPP