//
// Created by blomq on 2025-08-11.
//

#ifndef ROGUELIKE_SDL_RAII_HPP
#define ROGUELIKE_SDL_RAII_HPP

#include <memory>
#include <SDL3/SDL.h>

namespace psyengine
{
    struct SdlWindowDestroyer
    {
        void operator()(SDL_Window* window) const
        {
            SDL_DestroyWindow(window);
        }
    };

    struct SdlRendererDestroyer
    {
        void operator()(SDL_Renderer* renderer) const
        {
            SDL_DestroyRenderer(renderer);
        }
    };

    struct SdlTextureDestroyer
    {
        void operator()(SDL_Texture* texture) const
        {
            SDL_DestroyTexture(texture);
        }
    };

    struct SdlSurfaceDestroyer
    {
        void operator()(SDL_Surface* surface) const
        {
            SDL_DestroySurface(surface);
        }
    };

    inline std::shared_ptr<SDL_Texture> CreateSharedTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surface)
    {
        return {SDL_CreateTextureFromSurface(renderer, surface), SdlTextureDestroyer()};
    }

    using SdlWindowPtr = std::unique_ptr<SDL_Window, SdlWindowDestroyer>;
    using SdlRendererPtr = std::unique_ptr<SDL_Renderer, SdlRendererDestroyer>;
    using SdlTexturePtr = std::unique_ptr<SDL_Texture, SdlTextureDestroyer>;
    using SdlSurfacePtr = std::unique_ptr<SDL_Surface, SdlSurfaceDestroyer>;
}

#endif //ROGUELIKE_SDL_RAII_HPP