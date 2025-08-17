//
// Created by blomq on 2025-08-16.
//

#ifndef PSYENGINE_TEXTURE_MANAGER_HPP
#define PSYENGINE_TEXTURE_MANAGER_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <SDL3/SDL_render.h>

namespace psyengine::resources
{

    class TextureManager
    {
    public:
        static TextureManager& instance();

        std::shared_ptr<SDL_Texture> loadTexture(const std::string& path, SDL_Renderer* renderer);

        TextureManager(const TextureManager& other) = delete;
        TextureManager(TextureManager&& other) noexcept = delete;
        TextureManager& operator=(const TextureManager& other) = delete;
        TextureManager& operator=(TextureManager&& other) noexcept = delete;

    private:
        TextureManager() = default;
        ~TextureManager() = default;

        std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> textures_;
    };
}

#endif //PSYENGINE_TEXTURE_MANAGER_HPP
