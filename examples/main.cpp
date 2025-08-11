#include <SDL3/SDL_main.h>
#include "SDL3/SDL.h"

#include "psyengine/sdl_game.hpp"

int main(int argc, char** argv)
{
    auto game = psyengine::SdlGame();
    if (!game.init("Test", 800, 600, false))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 1;
    }

    game.run(60, 10);
    return 0;
}
