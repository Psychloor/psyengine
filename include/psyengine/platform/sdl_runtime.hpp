//
// Created by blomq on 2025-08-11.
//

#ifndef PSYENGINE_SDL_GAME_HPP
#define PSYENGINE_SDL_GAME_HPP



#include <memory>
#include <string>

#include "sdl_raii.hpp"


namespace psyengine::platform
{
    /**
     * @class SdlRuntime
     * @brief SDL-backed application runtime that owns initialization, the main loop, input dispatch, and rendering.
     *
     * SdlRuntime encapsulates:
     * - SDL subsystem initialization/teardown (video, audio, TTF, mixer) and lifetime management of SDL_Window/SDL_Renderer.
     * - A main loop combining fixed-step updates (for deterministic simulation/physics) and variable-step updates for rendering.
     * - Event processing and advanced input handling, delegating input events to the input manager and forwarding all events
     *   to the state manager.
     * - Coordination with a state manager that hosts game/application states.
     *   SdlRuntime calls into it for handleEvent, fixedUpdate, update, and render.
     * - Lag control: caps the number of fixed updates per frame and drops excess steps while preserving the interpolation phase.
     *
     * Typical usage:
     * 1) Construct SdlRuntime with make_shared.
     * 2) Call init() to create the window/renderer and initialize SDL subsystems.
     * 3) Push your initial state to the state manager.
     * 4) Call run() with a desired fixed timestep (e.g., 1/60 s).
     * 5) On shutdown, the destructor cleans up SDL resources and subsystems.
     *
     * Threading: All methods are intended to be called from the main thread that owns the SDL context.
     * Ownership: The runtime owns its SDL window and renderer via RAII wrappers and will release them before SDL_Quit().
     */
    class SdlRuntime : public std::enable_shared_from_this<SdlRuntime>
    {
    public:
        struct FixedUpdateFrequency
        {
            explicit constexpr FixedUpdateFrequency(const size_t value = 60) :
                frequency(value)
            {}

            size_t frequency;
        };

        struct MaxFixedUpdatesPerTick
        {
            // ReSharper disable once CppDFAConstantParameter
            explicit constexpr MaxFixedUpdatesPerTick(const size_t value) :
                maxUpdates(value)
            {}

            size_t maxUpdates;
        };

        SdlRuntime() = default;
         ~SdlRuntime();

        /**
         * @brief Initializes SDL subsystems and creates the application window and renderer.
         *
         * Initializes core SDL subsystems (video, audio, events, gamepad) and font/audio helper libraries,
         * then creates a high-DPI window and a compatible renderer.
         *
         * @param title Window title.
         * @param width Initial window width in pixels.
         * @param height Initial window height in pixels.
         * @param resizeableWindow If true, the window is resizable.
         * @return true on success; false if any subsystem or window/renderer creation fails.
         */
         bool init(const std::string& title, int width, int height, bool resizeableWindow = false);

        /**
         * Runs the main game loop with a fixed update rate and variable frame rate rendering.
         *
         * This function executes the primary game loop, combining fixed time step updates for
         * deterministic logic and variable-time rendering updates. It handles game events, inputs,
         * fixed updates, variable updates, and rendering in a consistent and efficient manner.
         *
         * The loop operates based on the following:
         * - Fixed updates are processed at a constant interval defined by `fixedTimeStep`, ensuring consistency
         *   for physics calculations and other time-sensitive game logic.
         * - A cap on the maximum number of fixed updates per tick, specified by `maxFixedUpdatesPerTick`, is enforced
         *   to prevent excessive CPU usage or a potential infinite loop in cases of lag.
         * - The frame updates and rendering are still handled with a variable time step within a maximum frame time
         *   limit (`maxFrameTime`) to ensure smooth graphical performance.
         *
         * @param fixedUpdateFrequency The number of fixed updates per second.
         * @param maxFixedUpdatesPerTick The maximum allowed fixed updates to process in a single frame.
         * @param maxFrameTime The maximum frame time to cap the elapsed time between frames.
         */
         void run(FixedUpdateFrequency fixedUpdateFrequency,
                                  MaxFixedUpdatesPerTick maxFixedUpdatesPerTick = MaxFixedUpdatesPerTick(10),
                                  double maxFrameTime = 1.0);

        /// Sets the window title.
        /// @return true on success.
         bool setWindowTitle(const std::string& title) const;
        /// Sets the window size in pixels.
        /// @return true on success.
         bool setWindowSize(int width, int height) const;

        /// Toggles fullscreen mode.
        /// @return true on success.
         bool setWindowFullscreen(bool fullscreen) const;

        /// Enables/disables VSync on the window surface, if supported.
        /// @return true on success.
         bool setWindowVsync(bool vsync) const;

        /// Requests the main loop to stop at the next opportunity.
         void quit();

        /// @return true while the main loop is running.
         bool isRunning() const;

        /// @return true if the runtime is dropping fixed steps due to lag.
         bool isLagging() const;

        /// @return Raw SDL window handle (owned by this runtime).
         SDL_Window* window() const;

        /// @return Raw SDL renderer handle (owned by this runtime).
         SDL_Renderer* renderer() const;

        SdlRuntime(const SdlRuntime& other) = delete;

        SdlRuntime(SdlRuntime&& other) noexcept :
            std::enable_shared_from_this<SdlRuntime>(other),
            running_(other.running_),
            lagging_(other.lagging_),
            window_(std::move(other.window_)),
            renderer_(std::move(other.renderer_))
        {}

        SdlRuntime& operator=(const SdlRuntime& other) = delete;

        SdlRuntime& operator=(SdlRuntime&& other) noexcept
        {
            if (this == &other)
            {
                return *this;
            }
            std::enable_shared_from_this<SdlRuntime>::operator =(other);
            running_ = other.running_;
            lagging_ = other.lagging_;
            window_ = std::move(other.window_);
            renderer_ = std::move(other.renderer_);
            return *this;
        }

    private:
        /// Polls SDL events, forwards to input and state managers, and handles quit requests.
        void handleEvents();

        /// Forwards fixed-step update to the state manager.
        static void fixedUpdate(double deltaTime);

        /// Forwards variable-step update to the state manager.
        static void update(double deltaTime);

        /// Renders the current state with an interpolation factor in [0, 1].
        void render(float interpolationFactor) const;

        bool running_{}; ///< Main loop flag.
        bool lagging_{}; ///< Indicates we dropped fixed steps due to lag in the last frame.

        SdlWindowPtr window_ = nullptr;
        SdlRendererPtr renderer_ = nullptr;

    };
}

#endif //PSYENGINE_SDL_GAME_HPP
