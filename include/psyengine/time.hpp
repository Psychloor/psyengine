//
// Created by blomq on 2025-08-12.
//

#ifndef PSYENGINE_TIME_HPP
#define PSYENGINE_TIME_HPP

#include <chrono>

namespace psyengine::time
{
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::high_resolution_clock::time_point;


    /**
     * Retrieves the current time point using the high-resolution clock.
     *
     * @return The current time point as a `TimePoint` object.
     */
    [[nodiscard]] inline TimePoint Now() noexcept
    {
        return Clock::now();
    }

    /**
     * Calculates the elapsed duration between two time points and casts it to the specified duration type.
     *
     * @param from The starting time point.
     * @param to The ending time point.
     * @return The elapsed duration between the two time points as a duration of the specified type `Dur`.
     */
    template <typename Dur = std::chrono::duration<double>>
    [[nodiscard]] Dur ElapsedDur(const TimePoint& from, const TimePoint& to) noexcept
    {
        return std::chrono::duration_cast<Dur>(to - from);
    }


    /**
     * Computes the elapsed time in seconds since a specified time point.
     *
     * @param since The time point from which the elapsed time is measured.
     * @return The elapsed time in seconds as a double.
     */
    [[nodiscard]] inline double ElapsedSince(const TimePoint& since) noexcept
    {
        return ElapsedDur<>(since, Now()).count();
    }


    /**
     * Calculates the elapsed time in seconds between two specified time points.
     *
     * @param start The starting time point.
     * @param end The ending time point.
     * @return The elapsed time in seconds as a double.
     */
    [[nodiscard]] inline double Elapsed(const TimePoint& start, const TimePoint& end) noexcept
    {
        return ElapsedDur<>(start, end).count();
    }


    /**
     * Calculates the elapsed time in seconds between two time points,
     * ensuring the value does not exceed a specified maximum.
     *
     * @param start The starting time point.
     * @param end The ending time point.
     * @param maxSeconds The maximum allowable elapsed time in seconds. Defaults to 1.0.
     * @return The clamped elapsed time in seconds as a double.
     */
    [[nodiscard]] inline double ElapsedClamped(const TimePoint& start, const TimePoint& end,
                                               const double maxSeconds = 1.0) noexcept
    {
        const double dt = Elapsed(start, end);
        return dt > maxSeconds ? maxSeconds : dt;
    }
}

#endif //PSYENGINE_TIME_HPP
