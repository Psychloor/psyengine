//
// Created by blomq on 2025-08-12.
//

#ifndef ROGUELIKE_TIME_HPP
#define ROGUELIKE_TIME_HPP

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
    inline TimePoint Now()
    {
        return Clock::now();
    }

    /**
     * Computes the elapsed time in seconds since the provided time point.
     *
     * @param time The reference time point from which the elapsed duration will be computed.
     * @return The elapsed time in seconds as a double.
     */
    inline double ElapsedSince(const TimePoint& time)
    {
        return std::chrono::duration<double>(Now() - time).count();
    }

    /**
     * Calculates the elapsed time in seconds between two time points.
     *
     * @param from The starting point in time.
     * @param to The ending point in time.
     * @return The elapsed time in seconds as a double.
     */
    inline double Elapsed(const TimePoint& from, const TimePoint& to)
    {
        return std::chrono::duration<double>(to - from).count();
    }
}

#endif //ROGUELIKE_TIME_HPP
