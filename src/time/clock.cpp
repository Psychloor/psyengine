//
// Created by blomq on 2025-08-17.
//

#include "psyengine/time/clock.hpp"

namespace psyengine::time
{
    void Clock::start() noexcept
    {
        if (running_)
        {
            return;
        }

        running_ = true;
        start_ = now();
        end_ = start_;
    }

    void Clock::stop() noexcept
    {
        running_ = false;
        end_ = now();
    }

    void Clock::restart() noexcept
    {
        start_ = now();
        end_ = start_;
        running_ = true;
    }

    void Clock::reset() noexcept
    {
        start_ = now();
        end_ = start_;
        running_ = false;
    }

    bool Clock::isRunning() const noexcept
    {
        return running_;
    }

    Clock::Duration Clock::elapsed() const noexcept
    {
        if (running_)
        {
            return now() - start_;
        }

        return end_ - start_;
    }

    double Clock::elapsedSeconds() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::duration<double>>(elapsed()).count();
    }

    long long Clock::elapsedMilliseconds() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed()).count();
    }

    long long Clock::elapsedNanoseconds() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed()).count();
    }

    Clock::Duration Clock::elapsedSince(const TimePoint& timePoint) noexcept
    {
        return now() - timePoint;
    }

    Clock::TimePoint Clock::now() noexcept
    {
        return ClockType::now();
    }
}
