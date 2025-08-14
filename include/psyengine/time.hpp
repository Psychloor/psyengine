//
// Created by blomq on 2025-08-12.
//

#ifndef PSYENGINE_TIME_HPP
#define PSYENGINE_TIME_HPP

namespace psyengine::time
{
    static const double PERFORMANCE_TIMER_FREQUENCY = static_cast<double>(SDL_GetPerformanceFrequency());
    using TimePoint = Uint64;


    [[nodiscard]] constexpr TimePoint Min()
    {
        return 0;
    }

    /**
     * Retrieves the current performance counter-value.
     * The performance counter is typically used for precise timing and measuring elapsed time.
     *
     * @return The current performance counter-value as a TimePoint.
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
     * @return The elapsed time in seconds as a double.
     */
    [[nodiscard]] inline double Elapsed(const TimePoint& start, const TimePoint& end) noexcept
    {
        return static_cast<double>(end - start) / PERFORMANCE_TIMER_FREQUENCY;
    }

    /**
     * Calculates the elapsed time in seconds since a given time point.
     *
     * @param since The reference time point from which the elapsed time is calculated.
     * @return The elapsed time in seconds as a double.
     */
    [[nodiscard]] inline double ElapsedSince(const TimePoint& since) noexcept
    {
        return Elapsed(Now(), since);
    }

    /**
     * Computes the elapsed time between two time points and clamps the result to a maximum value.
     *
     * This function calculates the elapsed time using the provided start and end time points.
     * If the elapsed time exceeds the specified maximum duration, the result will be clamped
     * to the value of maxSeconds. The elapsed time is measured in seconds.
     *
     * @param start The starting time point.
     * @param end The ending time point.
     * @param maxSeconds The maximum allowable duration in seconds. Defaults to 1.0.
     * @return The elapsed time in seconds, clamped to maxSeconds if it exceeds the specified limit.
     */
    [[nodiscard]] inline double ElapsedClamped(const TimePoint& start, const TimePoint& end,
                                               const double maxSeconds = 1.0) noexcept
    {
        const double dt = Elapsed(start, end);
        return dt > maxSeconds ? maxSeconds : dt;
    }
}

#endif //PSYENGINE_TIME_HPP
