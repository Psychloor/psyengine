//
// Created by blomq on 2025-08-16.
//

#ifndef PSYENGINE_TEXTURE_MANAGER_HPP
#define PSYENGINE_TEXTURE_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <SDL3/SDL_render.h>

#include "psyengine_export.h"
#include "psyengine/platform/sdl_runtime.hpp"

namespace psyengine::resources
{

    class TextureManager
    {
    public:
        PSYENGINE_EXPORT static TextureManager& instance();

        PSYENGINE_EXPORT std::shared_ptr<SDL_Texture> loadTexture(const std::string& path, SDL_Renderer* renderer);

        TextureManager(const TextureManager& other) = delete;
        TextureManager(TextureManager&& other) noexcept = delete;
        TextureManager& operator=(const TextureManager& other) = delete;
        TextureManager& operator=(TextureManager&& other) noexcept = delete;

    private:
        PSYENGINE_EXPORT TextureManager() = default;
        PSYENGINE_EXPORT ~TextureManager() = default;

        std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> textures_;
    };
}

#endif //PSYENGINE_TEXTURE_MANAGER_HPP