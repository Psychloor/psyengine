//
// Created by blomq on 2025-08-11.
//

#ifndef PSYENGINE_SDL_RAII_HPP
#define PSYENGINE_SDL_RAII_HPP

#include <memory>

#include <SDL3/SDL.h>

#ifdef PSYENGINE_WITH_MIXER
#include <SDL3_mixer/SDL_mixer.h>
#endif

#ifdef PSYENGINE_WITH_TTF
#include <SDL3_ttf/SDL_ttf.h>
#endif

namespace psyengine::platform
{
    /**
     * A custom deleter for SDL_Window objects used in conjunction with smart pointers.
     * Ensures that SDL_Window resources are properly released using SDL_DestroyWindow when
     * the managed pointer goes out of scope.
     */
    struct SdlWindowDestroyer
    {
        void operator()(SDL_Window* window) const
        {
            SDL_DestroyWindow(window);
        }
    };

    /**
     * A custom deleter for managing the destruction of SDL_Renderer objects.
     * Ensures proper cleanup of an SDL_Renderer by calling SDL_DestroyRenderer
     * when the managed renderer goes out of scope.
     */
    struct SdlRendererDestroyer
    {
        void operator()(SDL_Renderer* renderer) const
        {
            SDL_DestroyRenderer(renderer);
        }
    };

    /**
     * A custom deleter for SDL_Texture objects, ensuring proper cleanup of textures when they are no longer needed.
     * This is designed to be used with smart pointers, such as std::shared_ptr or std::unique_ptr, for automatic
     * resource management of SDL_Texture instances.
     */
    struct SdlTextureDestroyer
    {
        void operator()(SDL_Texture* texture) const
        {
            SDL_DestroyTexture(texture);
        }
    };

    /**
     * A custom deleter for SDL_Surface objects. Ensures proper cleanup of
     * SDL_Surface resources by calling SDL_FreeSurface when the surface
     * is no longer in use.
     */
    struct SdlSurfaceDestroyer
    {
        void operator()(SDL_Surface* surface) const
        {
            SDL_DestroySurface(surface);
        }
    };

    /**
     * A custom deleter for SDL_AudioDeviceID objects. Ensures that SDL audio device
     * resources are properly released by calling SDL_CloseAudioDevice when the managed
     * audio device goes out of scope.
     */
    struct SdlAudioDeviceDestroyer
    {
        void operator()(const SDL_AudioDeviceID device) const
        {
            SDL_CloseAudioDevice(device);
        }
    };

    using SdlWindowPtr = std::unique_ptr<SDL_Window, SdlWindowDestroyer>;
    using SdlRendererPtr = std::unique_ptr<SDL_Renderer, SdlRendererDestroyer>;
    using SdlTexturePtr = std::unique_ptr<SDL_Texture, SdlTextureDestroyer>;
    using SdlSurfacePtr = std::unique_ptr<SDL_Surface, SdlSurfaceDestroyer>;

#ifdef PSYENGINE_WITH_MIXER
    /**
     * A custom deleter for MIX_Mixer objects, used to manage the lifetime of audio mixer resources.
     * Ensures the proper cleanup of MIX_Mixer by invoking MIX_DestroyMixer when the mixer is no longer needed.
     * Designed to be used with smart pointers for automatic resource management of audio mixers in SDL_mixer.
     */
    struct SdlMixerDestroyer
    {
        void operator()(MIX_Mixer* mixer) const
        {
            MIX_DestroyMixer(mixer);
        }
    };

    using SdlMixerPtr = std::unique_ptr<MIX_Mixer, SdlMixerDestroyer>;
#endif

#ifdef PSYENGINE_WITH_TTF
    /**
     * A custom deleter used to manage the lifetime of SDL_ttf resources, such as TTF_Font.
     * Ensures that the associated SDL_ttf resource is properly released when it is no longer necessary.
     *
     * Designed to be used with smart pointers for automatic memory management of SDL_ttf objects.
     */
    struct SdlTtfDestroyer
    {
        void operator()(TTF_Font* font) const
        {
            TTF_CloseFont(font);
        }
    };

    using SdlTtfPtr = std::unique_ptr<TTF_Font, SdlTtfDestroyer>;
#endif

}

#endif //PSYENGINE_SDL_RAII_HPP
