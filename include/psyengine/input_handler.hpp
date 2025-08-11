//
// Created by blomq on 2025-08-11.
//

#ifndef PSYENGINE_INPUT_HANDLER_HPP
#define PSYENGINE_INPUT_HANDLER_HPP

#include <array>

#include "SDL3/SDL_scancode.h"

namespace psyengine
{
    class InputHandler
    {
    public:
        static InputHandler& instance();

        void updateInputState();

        [[nodiscard]] bool isKeyPressed(SDL_Scancode scancode) const;
        [[nodiscard]] bool isKeyReleased(SDL_Scancode scancode) const;
        [[nodiscard]] bool isKeyHeld(SDL_Scancode scancode) const;
        [[nodiscard]] bool isKeyJustPressed(SDL_Scancode scancode) const;
        [[nodiscard]] bool isKeyJustReleased(SDL_Scancode scancode) const;

        InputHandler(const InputHandler&) = delete;
        InputHandler& operator=(const InputHandler&) = delete;
        InputHandler(InputHandler&&) = delete;
        InputHandler& operator=(InputHandler&&) = delete;
    private:
        InputHandler();
        ~InputHandler() = default;

        std::array<bool, SDL_SCANCODE_COUNT> currentState_;
        std::array<bool, SDL_SCANCODE_COUNT> previousState_;
    };
}

#endif //PSYENGINE_INPUT_HANDLER_HPP