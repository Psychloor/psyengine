//
// Created by blomq on 2025-08-11.
//

#include "psyengine/sdl_game.hpp"

#include <iostream>

#include "psyengine/input_handler.hpp"
#include "psyengine/state_manager.hpp"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"

namespace psyengine
{

    SdlGame::~SdlGame()
    {
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
            return false;
        }

        if (!TTF_Init())
        {
            return false;
        }

        if (!MIX_Init())
        {
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
            return false;
        }

        window_ = SdlWindowPtr(windowTemp);
        renderer_ = SdlRendererPtr(rendererTemp);

        return true;
    }

    void SdlGame::run(const uint16_t fixedUpdateFrequency, const size_t maxUpdatesPerTick = 10)
    {
        if (fixedUpdateFrequency == 0)
        {
            std::cerr << "run() called with fixedUpdateFrequency == 0\n";
            return;
        }
        if (maxUpdatesPerTick == 0)
        {
            std::cerr << "run() called with maxUpdatesPerTick == 0\n";
            return;
        }

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
            double frameDelta = static_cast<double>(now - lastCounter) / static_cast<double>(freq);
            lastCounter = now;

            // Clamp very large spikes (alt-tab, breakpoint, etc.)
            frameDelta = std::min(frameDelta, 1.0);

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
            // const float alpha = static_cast<float>(accumulatedTime / tickPeriod);
            render();

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
                StateManager::getInstance().handleEvent(&event);
                break;
            }
        }
    }

    void SdlGame::fixedUpdate(const float deltaTime)
    {
        StateManager::getInstance().fixedUpdate(deltaTime);
    }

    void SdlGame::update(const double deltaTime)
    {
        StateManager::getInstance().update(deltaTime);
    }

    void SdlGame::render() const
    {
        SDL_SetRenderDrawColorFloat(renderer_.get(), 0.392f, 0.584f, 0.929f, 1.0f);
        SDL_RenderClear(renderer_.get());
        SDL_SetRenderDrawColorFloat(renderer_.get(), 1.0f, 1.0f, 1.0f, 1.0f);

        StateManager::getInstance().render(renderer_.get());

        SDL_RenderPresent(renderer_.get());
    }
}
