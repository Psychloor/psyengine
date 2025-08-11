//
// Created by blomq on 2025-08-11.
//

#include "psyengine/sdl_game.hpp"

#include <cassert>
#include <iostream>

#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"

#include "psyengine/input_handler.hpp"
#include "psyengine/state_manager.hpp"

namespace psyengine
{

    SdlGame::~SdlGame()
    {
        StateManager::instance().clear();

        // Ensure SDL objects are destroyed before SDL_Quit
        renderer_.reset();
        window_.reset();

        // Close audio device (if opened) and quit subsystems
        //Mix_CloseAudio();
        MIX_Quit();
        TTF_Quit();
        SDL_Quit();
    }

    bool SdlGame::init(const std::string_view title, const int width, const int height, const bool resizeableWindow)
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL_GetError());
            return false;
        }

        if (!TTF_Init())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_Init failed: %s", SDL_GetError());
            return false;
        }

        if (!MIX_Init())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MIX_Init failed: %s", SDL_GetError());
            return false;
        }


        SDL_WindowFlags windowFlags = SDL_WINDOW_HIGH_PIXEL_DENSITY;
        if (resizeableWindow)
        {
            windowFlags |= SDL_WINDOW_RESIZABLE;
        }

        SDL_Window* windowTemp;
        SDL_Renderer* rendererTemp;
        if (!SDL_CreateWindowAndRenderer(title.data(), width, height, windowFlags, &windowTemp,
                                         &rendererTemp))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
            return false;
        }

        window_ = raii::SdlWindowPtr(windowTemp);
        renderer_ = raii::SdlRendererPtr(rendererTemp);

        return true;
    }

    void SdlGame::run(const uint16_t fixedUpdateFrequency, const size_t maxUpdatesPerTick = 10)
    {
        assert(fixedUpdateFrequency > 0 && "Fixed update frequency must be greater than 0");
        assert(maxUpdatesPerTick > 0 && "Max updates per tick must be greater than 0");

        const size_t maxUpdates = std::max<size_t>(1, maxUpdatesPerTick);
        running_ = true;
        const double tickPeriod = 1.0 / static_cast<double>(fixedUpdateFrequency);

        double accumulatedTime = 0.0;
        size_t accumulatedUpdates = 0;

        const uint64_t freq = SDL_GetPerformanceFrequency();
        uint64_t lastCounter = SDL_GetPerformanceCounter();

        uint64_t lastLagWarnCounter = 0;

        while (running_)
        {
            const uint64_t now = SDL_GetPerformanceCounter();

            // Clamp very large spikes (alt-tab, breakpoint, etc.)
            const double frameDelta = std::min(static_cast<double>(now - lastCounter) / static_cast<double>(freq), 1.0);
            lastCounter = now;
            accumulatedTime += frameDelta;

            // Events first, then snapshot input for this frame
            handleEvents();
            InputHandler::instance().updateInputState();

            // Fixed updates
            while (accumulatedTime >= tickPeriod && accumulatedUpdates < maxUpdates)
            {
                accumulatedTime -= tickPeriod;
                ++accumulatedUpdates;
                fixedUpdate(static_cast<float>(tickPeriod));
            }

            if (accumulatedTime >= tickPeriod)
            {
                // We’re still behind; drop extra lag but keep phase remainder
                const double remainder = std::fmod(accumulatedTime, tickPeriod);
                accumulatedTime = remainder;
                lagging_ = true;

                // Throttle warning to 1/sec
                if ((now - lastLagWarnCounter) > freq)
                {
                    SDL_Log("Warning: fixed update lagging, dropped extra steps");
                    lastLagWarnCounter = now;
                }
            }
            else
            {
                // No lag
                lastLagWarnCounter = 0;
                lagging_ = false;
            }

            accumulatedUpdates = 0;

            // Variable-step update for render-side logic
            update(frameDelta);

            // Interpolation factor for smooth rendering
            const auto alpha = static_cast<float>(accumulatedTime / tickPeriod);
            render(alpha);

            // Yield a bit to reduce cpu usage
            SDL_Delay(1);
        }
    }

    bool SdlGame::setWindowTitle(const std::string_view title) const
    {
        return SDL_SetWindowTitle(window_.get(), title.data());
    }

    bool SdlGame::setWindowSize(const int width, const int height) const
    {
        return SDL_SetWindowSize(window_.get(), width, height);
    }

    bool SdlGame::setWindowFullscreen(const bool fullscreen) const
    {
        return SDL_SetWindowFullscreen(window_.get(), fullscreen);
    }

    bool SdlGame::setWindowVsync(const bool vsync) const
    {
        return SDL_SetWindowSurfaceVSync(window_.get(), vsync);
    }

    void SdlGame::quit()
    {
        running_ = false;
    }

    bool SdlGame::isRunning() const
    {
        return running_;
    }

    bool SdlGame::isLagging() const
    {
        return lagging_;
    }

    SDL_Window* SdlGame::window() const
    {
        return window_.get();
    }

    SDL_Renderer* SdlGame::renderer() const
    {
        return renderer_.get();
    }

    void SdlGame::handleEvents()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running_ = false;
                return;

            default:
                StateManager::instance().handleEvent(&event);
                break;
            }
        }
    }

    void SdlGame::fixedUpdate(const float deltaTime)
    {
        StateManager::instance().fixedUpdate(deltaTime);
    }

    void SdlGame::update(const double deltaTime)
    {
        StateManager::instance().update(deltaTime);
    }

    void SdlGame::render(const float alpha) const
    {
        // CornFlowerBlue
        SDL_SetRenderDrawColorFloat(renderer_.get(), 0.392f, 0.584f, 0.929f, 1.0f);
        SDL_RenderClear(renderer_.get());
        SDL_SetRenderDrawColorFloat(renderer_.get(), 1.0f, 1.0f, 1.0f, 1.0f);

        StateManager::instance().render(renderer_.get(), alpha);

        SDL_RenderPresent(renderer_.get());
    }
}
