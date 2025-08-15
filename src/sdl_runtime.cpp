//
// Created by blomq on 2025-08-11.
//

#include "psyengine/sdl_runtime.hpp"

#ifdef PSYENGINE_WITH_MIXER
#include <SDL3_mixer/SDL_mixer.h>
#endif

#ifdef PSYENGINE_WITH_TTF
#include <SDL3_ttf/SDL_ttf.h>
#endif

#include "psyengine/input_manager.hpp"
#include "psyengine/state_manager.hpp"
#include "psyengine/time.hpp"

namespace psyengine
{
    SdlRuntime::~SdlRuntime()
    {
        StateManager::instance().clear();

        // Ensure SDL objects are destroyed before SDL_Quit
        renderer_.reset();
        window_.reset();

#ifdef PSYENGINE_WITH_MIXER
        MIX_Quit();
#endif

#ifdef PSYENGINE_WITH_TTF
        TTF_Quit();
#endif

        SDL_Quit();
    }

    bool SdlRuntime::init(const std::string& title, const int width, const int height, const bool resizeableWindow)
    {
        constexpr auto initFlags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD;
        if (!SDL_Init(initFlags))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL_GetError());
            return false;
        }

#ifdef PSYENGINE_WITH_TTF
        if (!TTF_Init())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_Init failed: %s", SDL_GetError());
            return false;
        }
#endif

#ifdef PSYENGINE_WITH_MIXER
        if (!MIX_Init())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MIX_Init failed: %s", SDL_GetError());
            return false;
        }
#endif

        SDL_WindowFlags windowFlags = SDL_WINDOW_HIGH_PIXEL_DENSITY;
        if (resizeableWindow)
        {
            windowFlags |= SDL_WINDOW_RESIZABLE;
        }

        SDL_Window* window;
        SDL_Renderer* renderer;
        if (!SDL_CreateWindowAndRenderer(title.c_str(), width, height, windowFlags, &window, &renderer))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
            return false;
        }

        window_ = raii::SdlWindowPtr(window);
        renderer_ = raii::SdlRendererPtr(renderer);

        return true;
    }


    void SdlRuntime::run(const size_t fixedUpdateFrequency, const size_t maxFixedUpdatesPerTick, const double maxFrameTime) // NOLINT(*-easily-swappable-parameters)
    {
        SDL_assert(maxFixedUpdatesPerTick > 0); // NOLINT(*-else-after-return)

        const size_t maxUpdates = std::max<size_t>(1, maxFixedUpdatesPerTick);
        const double tickPeriod = 1.0 / static_cast<double>(fixedUpdateFrequency);
        const double maxFrameTimeSeconds = maxFrameTime;

        double accumulatedTime = 0.0;
        size_t accumulatedUpdates = 0;

        time::TimePoint lastTime = time::Now();
        time::TimePoint lastLagWarnTime = time::Min();

        running_ = true;
        while (running_)
        {
            const time::TimePoint now = time::Now();
            const double frameDelta = std::min(time::Elapsed<double>(lastTime, now), maxFrameTimeSeconds);

            lastTime = now;
            accumulatedTime += frameDelta;

            // Events first, then update input for this frame
            handleEvents();
            InputManager::instance().update();

            // Fixed updates
            while (accumulatedTime >= tickPeriod && accumulatedUpdates < maxUpdates)
            {
                accumulatedTime -= tickPeriod;
                ++accumulatedUpdates;
                fixedUpdate(tickPeriod);
            }

            // Check for lag
            if (accumulatedTime >= tickPeriod)
            {
                // We’re still behind; drop extra lag but keep phase remainder
                const double remainder = std::fmod(accumulatedTime, tickPeriod);
                accumulatedTime = remainder;
                lagging_ = true;

                // Throttle warning to 1/sec
                if (time::Elapsed<double>(lastLagWarnTime, now) > 1.0)
                {
                    SDL_Log("Warning: fixed update lagging, dropped extra steps");
                    lastLagWarnTime = now;
                }
            }
            else
            {
                // No lag
                lastLagWarnTime = now;
                lagging_ = false;
            }

            accumulatedUpdates = 0;

            // Variable-step update for render-side logic
            update(frameDelta);

            // Interpolation factor for smooth rendering
            const auto interpolationFactor = static_cast<float>(accumulatedTime / tickPeriod);
            render(interpolationFactor);

            // Yield a bit to reduce cpu usage
            SDL_Delay(1);
        }
    }

    bool SdlRuntime::setWindowTitle(const std::string& title) const
    {
        return SDL_SetWindowTitle(window_.get(), title.c_str());
    }

    bool SdlRuntime::setWindowSize(const int width, const int height) const
    {
        return SDL_SetWindowSize(window_.get(), width, height);
    }

    bool SdlRuntime::setWindowFullscreen(const bool fullscreen) const
    {
        return SDL_SetWindowFullscreen(window_.get(), fullscreen);
    }

    bool SdlRuntime::setWindowVsync(const bool vsync) const
    {
        return SDL_SetWindowSurfaceVSync(window_.get(), vsync ? 1 : SDL_WINDOW_SURFACE_VSYNC_DISABLED);
    }

    void SdlRuntime::quit()
    {
        running_ = false;
    }

    bool SdlRuntime::isRunning() const
    {
        return running_;
    }

    bool SdlRuntime::isLagging() const
    {
        return lagging_;
    }

    SDL_Window* SdlRuntime::window() const
    {
        return window_.get();
    }

    SDL_Renderer* SdlRuntime::renderer() const
    {
        return renderer_.get();
    }

    void SdlRuntime::handleEvents()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running_ = false;
                return;

            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
            case SDL_EVENT_MOUSE_MOTION:
            case SDL_EVENT_MOUSE_WHEEL:
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            case SDL_EVENT_GAMEPAD_REMOVED:
                // Fall through in case people want to use these anyway
                InputManager::instance().handleEvent(event);
                [[fallthrough]];
            default:
                StateManager::instance().handleEvent(event);
                break;
            }
        }
    }

    void SdlRuntime::fixedUpdate(const double deltaTime)
    {
        StateManager::instance().fixedUpdate(deltaTime);
    }

    void SdlRuntime::update(const double deltaTime)
    {
        StateManager::instance().update(deltaTime);
    }

    void SdlRuntime::render(const float interpolationFactor) const
    {
        // CornFlowerBlue
        SDL_SetRenderDrawColorFloat(renderer_.get(), 0.392f, 0.584f, 0.929f, 1.0f);
        SDL_RenderClear(renderer_.get());
        SDL_SetRenderDrawColorFloat(renderer_.get(), 1.0f, 1.0f, 1.0f, 1.0f);

        StateManager::instance().render(renderer_.get(), interpolationFactor);

        SDL_RenderPresent(renderer_.get());
    }
}
