//
// Created by blomq on 2025-08-11.
//

#ifndef PSYENGINE_SDL_RAII_HPP
#define PSYENGINE_SDL_RAII_HPP

#include <memory>

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace psyengine::raii
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

    struct SdlMixerDestroyer
    {
        void operator()(MIX_Mixer* mixer) const
        {
            MIX_DestroyMixer(mixer);
        }
    };

    struct SdlTtfDestroyer
    {
        void operator()(TTF_Font* font) const
        {
            TTF_CloseFont(font);
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

    using SdlMixerPtr = std::unique_ptr<MIX_Mixer, SdlMixerDestroyer>;

    using SdlTtfPtr = std::unique_ptr<TTF_Font, SdlTtfDestroyer>;
}

#endif //PSYENGINE_SDL_RAII_HPP