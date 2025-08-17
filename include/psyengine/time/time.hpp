//
// Created by blomq on 2025-08-12.
//

#ifndef PSYENGINE_TIMER_HPP
#define PSYENGINE_TIMER_HPP

#include <concepts>
#include <limits>

#include <SDL3/SDL_timer.h>

namespace psyengine::time
{
    using TimePoint = Uint64;

    /**
     * Retrieves the performance counter frequency (ticks per second).
     * The value is cached on the first call.
     */
    [[nodiscard]] inline TimePoint PerformanceFrequency() noexcept
    {
        static const TimePoint FREQ = SDL_GetPerformanceFrequency();
        return FREQ;
    }

    /**
     * Retrieves the current performance counter-value.
     */
    [[nodiscard]] inline TimePoint Now() noexcept
    {
        return SDL_GetPerformanceCounter();
    }

    /**
     * Calculates the elapsed time, in seconds, between two time points.
     *
     * @param start The starting time point.
     * @param end The ending time point.
     * @return The elapsed time in seconds as T.
     */
    template <std::floating_point T = double>
    [[nodiscard]] T Elapsed(const TimePoint start, const TimePoint end) noexcept
    {
        return static_cast<T>(end - start) / static_cast<T>(PerformanceFrequency());
    }

    /**
        * Calculates the elapsed time in seconds since a given time point.
        *
        * @param since The reference time point from which the elapsed time is calculated.
        * @return The elapsed time in seconds as T.
        */
    template <std::floating_point T = double>
    [[nodiscard]] T ElapsedSince(const TimePoint since) noexcept
    {
        return Elapsed<T>(since, Now());
    }

    /**
     * Computes the elapsed time between two time points and clamps the result to a maximum value.
     *
     * @param start The starting time point.
     * @param end The ending time point.
     * @param maxSeconds The maximum allowable duration in seconds (same type as return).
     * @return The elapsed time in seconds, clamped to maxSeconds.
     */
    template <std::floating_point T = double>
    [[nodiscard]] T ElapsedClamped(const TimePoint start, const TimePoint end,
                                   T maxSeconds = static_cast<T>(1)) noexcept
    {
        const T dt = Elapsed<T>(start, end);
        return dt > maxSeconds ? maxSeconds : dt;
    }

    /**
     * Convert raw tick count to seconds.
     */
    template <std::floating_point T = double>
    [[nodiscard]] T TicksToSeconds(TimePoint ticks) noexcept
    {
        return static_cast<T>(ticks) / static_cast<T>(PerformanceFrequency());
    }

    /**
     * Convert seconds to raw tick count (rounded down).
     */
    template <std::floating_point T = double>
    [[nodiscard]] TimePoint SecondsToTicks(T seconds) noexcept
    {
        return static_cast<TimePoint>(seconds * static_cast<T>(PerformanceFrequency()));
    }

    /**
     * Retrieves the maximum possible value for a TimePoint.
     */
    [[nodiscard]] constexpr TimePoint Max() noexcept
    {
        return std::numeric_limits<TimePoint>::max();
    }

    /**
     * Retrieves the minimum possible value of a TimePoint.
     */
    [[nodiscard]] constexpr TimePoint Min() noexcept
    {
        return std::numeric_limits<TimePoint>::min();
    }
}

#endif //PSYENGINE_TIMER_HPP
