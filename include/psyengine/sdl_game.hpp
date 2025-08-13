//
// Created by blomq on 2025-08-11.
//

#ifndef PSYENGINE_SDL_GAME_HPP
#define PSYENGINE_SDL_GAME_HPP

#include <chrono>

#include "psyengine/sdl_raii.hpp"

namespace psyengine
{
    class SdlGame : public std::enable_shared_from_this<SdlGame>
    {
    public:
        SdlGame() = default;
        ~SdlGame();

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
         * @param fixedTimeStep The fixed time interval for updates (e.g., 1/60 seconds for 60 updates per second).
         * @param maxFixedUpdatesPerTick The maximum allowed fixed updates to process in a single frame.
         * @param maxFrameTime The maximum frame time to cap the elapsed time between frames.
         */
        void run(std::chrono::duration<double> fixedTimeStep, size_t maxFixedUpdatesPerTick = 10, std::chrono::duration<double> maxFrameTime = std::chrono::seconds(1));

        bool setWindowTitle(const std::string& title) const;
        bool setWindowSize(int width, int height) const;
        bool setWindowFullscreen(bool fullscreen) const;
        bool setWindowVsync(bool vsync) const;

        void quit();

        bool isRunning() const;
        bool isLagging() const;
        SDL_Window* window() const;
        SDL_Renderer* renderer() const;

    private:
        void handleEvents();
        static void fixedUpdate(double deltaTime);
        static void update(double deltaTime);
        void render(float alpha) const;

        bool running_{};
        bool lagging_{};

        raii::SdlWindowPtr window_;
        raii::SdlRendererPtr renderer_;
    };
}

#endif //PSYENGINE_SDL_GAME_HPP
