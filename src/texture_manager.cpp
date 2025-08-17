//
// Created by blomq on 2025-08-16.
//

#include "psyengine/resources/texture_manager.hpp"

#include <stdexcept>
#include <SDL3_image/SDL_image.h>

#include "psyengine/debug/assert.hpp"

namespace psyengine::resources
{
    TextureManager& TextureManager::instance()
    {
        static TextureManager inst;
        return inst;
    }

    std::shared_ptr<SDL_Texture> TextureManager::loadTexture(const std::string& path, SDL_Renderer* renderer)
    {
        PSY_DEBUG_ASSERT(renderer != nullptr, "Renderer is null");
        PSY_DEBUG_ASSERT(!path.empty(), "Path is empty");

        const auto it = textures_.find(path);
        if (it != std::end(textures_))
        {
            return it->second;
        }

        const auto texture = IMG_LoadTexture(renderer, path.c_str());
        if (texture == nullptr)
        {
            return nullptr;
        }

        auto texturePtr = std::shared_ptr<SDL_Texture>(texture, SDL_DestroyTexture);
        textures_[path] = texturePtr;
        return texturePtr;
    }
}