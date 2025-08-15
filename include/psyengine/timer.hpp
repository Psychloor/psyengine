//
// Created by blomq on 2025-08-12.
//

#ifndef PSYENGINE_TIMER_HPP
#define PSYENGINE_TIMER_HPP

#include "psyengine_export.h"

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
    [[nodiscard]] PSYENGINE_EXPORT inline TimePoint PerformanceFrequency() noexcept
    {
        static const TimePoint FREQ = SDL_GetPerformanceFrequency();
        return FREQ;
    }

    /**
     * Retrieves the current performance counter-value.
     */
    [[nodiscard]] PSYENGINE_EXPORT inline TimePoint Now() noexcept
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

    class PSYENGINE_EXPORT DeltaTimer {
    public:
        template<std::floating_point T = float>
        [[nodiscard]] T getDelta() noexcept {
            const TimePoint current = Now();
            const T delta = Elapsed<T>(lastTime_, current);
            lastTime_ = current;
            return delta;
        }

        template<std::floating_point T = float>
        [[nodiscard]] T getDeltaClamped(T maxDelta = static_cast<T>(1.0/30.0)) noexcept {
            const TimePoint current = Now();
            const T delta = ElapsedClamped<T>(lastTime_, current, maxDelta);
            lastTime_ = current;
            return delta;
        }

    private:
        TimePoint lastTime_ = Now();
    };

    class PSYENGINE_EXPORT Timer {
    public:
        void start() noexcept {
            startTime_ = Now();
            running_ = true;
        }

        void reset() noexcept {
            startTime_ = Now();
        }

        template<std::floating_point T = float>
        [[nodiscard]] T elapsed() const noexcept {
            return running_ ? ElapsedSince<T>(startTime_) : T{0};
        }

        [[nodiscard]] bool isRunning() const noexcept {
            return running_;
        }

        void stop() noexcept {
            running_ = false;
        }

    private:
    private:
        TimePoint startTime_ = 0;
        bool running_ = false;
    };
}

#endif //PSYENGINE_TIMER_HPP
