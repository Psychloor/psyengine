#include <chrono>

#include <SDL3/SDL_main.h>
#include "SDL3/SDL.h"

#include "psyengine/sdl_runtime.hpp"

int main(int argc, char** argv)
{
    auto game = psyengine::SdlRuntime();
    if (!game.init("Test", 800, 600, false))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Push an initial state with psyengine::StateManager::instance().pushState()


    constexpr int fixedUpdateFrequency = 60;
    constexpr int maxFixedUpdatesPerTick = 10;
    constexpr auto fixedTimeStep = std::chrono::duration<double>(1.0 / static_cast<double>(fixedUpdateFrequency));

    game.run(fixedTimeStep, maxFixedUpdatesPerTick);
    return 0;
}
