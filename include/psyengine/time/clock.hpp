//
// Created by blomq on 2025-08-17.
//

#ifndef PSYENGINE_CLOCK_HPP
#define PSYENGINE_CLOCK_HPP



#include <chrono>

namespace psyengine::time
{
    /**
     * @class Clock
     * @brief Provides functionality to measure elapsed time.
     *
     * The Clock class offers methods to start, stop, reset, and restart a timer,
     * as well as retrieve elapsed time in various units (seconds, milliseconds, nanoseconds).
     */
    class Clock
    {
        using ClockType = std::chrono::high_resolution_clock;
        using TimePoint = ClockType::time_point;
        using Duration = ClockType::duration;

    public:
        /**
         * @brief Default constructor for the Clock class.
         *
         * Initializes a Clock object with default settings.
         * This constructor does not perform any additional setup actions.
         */
         Clock() = default;

        /**
         * @brief Starts the clock if it is not already running.
         *
         * Marks the current timestamp as the starting point of the clock
         * and sets the clock to a running state. If the clock is already running,
         * this method has no effect.
         */
         void start() noexcept;

        /**
         * @brief Stops the clock and records the end time.
         *
         * Disables the clock's running state and captures the current time as the endpoint,
         * enabling the measurement of the elapsed time between the start and stop operations.
         */
         void stop() noexcept;

        /**
         * @brief Restarts the clock by resetting its start and end time points to the current time
         *        and marking it as running.
         *
         * This function initializes or re-initializes the clock's time points and sets its state
         * to running. Any previously recorded elapsed time is cleared.
         *
         * @note The clock will reflect elapsed time starting from the moment this function is called.
         *
         * @thread_safety This method is not thread-safe and should not be called concurrently on the
         * same clock instance.
         */
         void restart() noexcept;

        /**
         * @brief Resets the clock, setting the `start_` and `end_` time points to the current time
         *        and marking the clock as not running.
         *
         * The function effectively clears the clock's state. After this function is called:
         * - The elapsed time will be considered zero.
         * - The clock will be in a stopped state (`isRunning()` will return false).
         *
         * This is useful for situations where the clock's usage is to be restarted entirely
         * without immediately starting the timing operation.
         *
         * @note This function does not start the clock; it only resets its state.
         */
         void reset() noexcept;

        /**
         * Checks whether the clock is currently running.
         *
         * @return true if the clock is running, false otherwise.
         */
         [[nodiscard]]  bool isRunning() const noexcept;

        /**
         * @brief Returns the elapsed time since the clock started.
         *
         * This function calculates the duration between when the clock started
         * and either the current time (if the clock is running) or the time
         * when the clock was last stopped.
         *
         * @return Duration The time duration that has elapsed since the clock
         *         started. If the clock is running, the duration is calculated
         *         as the difference between the current time and the starting
         *         time. Otherwise, it is the difference between the stopping
         *         time and the starting time.
         */
        [[nodiscard]]  Duration elapsed() const noexcept;

        /**
         * Calculates the total elapsed time in seconds.
         *
         * This method returns the elapsed time between the clock's starting point and
         * the current time if the clock is running, or between the starting point
         * and the stopping point if the clock has been stopped. The returned value is
         * represented in seconds with double precision.
         *
         * @return The elapsed time in seconds as a double.
         */
        [[nodiscard]]  double elapsedSeconds() const noexcept;

        /**
         * Calculates the total elapsed time in milliseconds since the clock was started.
         * If the clock is currently running, the elapsed time is calculated from the start time to the current time.
         * If the clock is stopped, the elapsed time is calculated from the start time to the time it was last stopped.
         *
         * @return The elapsed time in milliseconds as a signed integer.
         */
        [[nodiscard]]  long long elapsedMilliseconds() const noexcept;

        /**
         * Returns the time elapsed in nanoseconds since the last call to start().
         *
         * If the clock is running, calculates the duration from the initial start time
         * to the current time. If the clock is stopped, calculates the duration from
         * the initial start time to the last recorded stop time.
         *
         * @return The elapsed time in nanoseconds as a signed integer.
         * @note The result is computed using the std::chrono::nanoseconds duration.
         */
        [[nodiscard]]  long long elapsedNanoseconds() const noexcept;

        /**
         * Calculates the duration elapsed since the specified time point.
         *
         * @param timePoint A reference to the time point from which the elapsed duration is to be calculated.
         * @return The duration that has elapsed since the given time point.
         */
        [[nodiscard]]  static Duration elapsedSince(const TimePoint& timePoint) noexcept;

        /**
         * @brief Provides the current time point based on the high-resolution clock.
         *
         * Uses the underlying ClockType (std::chrono::high_resolution_clock) to
         * retrieve the current point in time. Useful for getting timestamps or
         * calculating durations by comparing time points.
         *
         * @return The current time point as a ClockType::time_point object.
         */
        [[nodiscard]]  static TimePoint now() noexcept;

    private:
        TimePoint start_, end_;
        bool running_ = false;
    };
}

#endif //PSYENGINE_CLOCK_HPP
