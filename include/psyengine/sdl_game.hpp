//
// Created by blomq on 2025-08-11.
//

#ifndef PSYENGINE_SDL_GAME_HPP
#define PSYENGINE_SDL_GAME_HPP

#include <string_view>

#include "psyengine/sdl_raii.hpp"

namespace psyengine
{
    class SdlGame : public std::enable_shared_from_this<SdlGame>
    {
    public:
        SdlGame() = default;
        ~SdlGame();

        bool init(std::string_view title, int width, int height, bool resizeableWindow = false);
        void run(uint16_t fixedUpdateFrequency, size_t maxUpdatesPerTick);

        bool setWindowTitle(std::string_view title) const;
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
        static void fixedUpdate(float deltaTime);
        static void update(double deltaTime);
        void render(float alpha) const;

        bool running_{};
        bool lagging_{};

        raii::SdlWindowPtr window_;
        raii::SdlRendererPtr renderer_;
    };
}

#endif //PSYENGINE_SDL_GAME_HPP
