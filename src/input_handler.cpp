//
// Created by blomq on 2025-08-11.
//

#include "psyengine/input_handler.hpp"

#include "SDL3/SDL_keyboard.h"

namespace psyengine
{
    InputHandler& InputHandler::instance()
    {
        static InputHandler instance;
        return instance;
    }

    void InputHandler::updateInputState()
    {
        std::ranges::swap(previousState_, currentState_);

        const bool* keyboard = SDL_GetKeyboardState(nullptr);
        std::copy_n(keyboard, SDL_SCANCODE_COUNT, currentState_.begin());
    }

    bool InputHandler::isKeyPressed(const SDL_Scancode scancode) const
    {
        return currentState_[scancode];
    }

    bool InputHandler::isKeyReleased(const SDL_Scancode scancode) const
    {
        return !currentState_[scancode];
    }

    bool InputHandler::isKeyHeld(const SDL_Scancode scancode) const
    {
        return currentState_[scancode] && previousState_[scancode];
    }

    bool InputHandler::isKeyJustPressed(const SDL_Scancode scancode) const
    {
        return !previousState_[scancode] && currentState_[scancode];
    }

    bool InputHandler::isKeyJustReleased(const SDL_Scancode scancode) const
    {
        return previousState_[scancode] && !currentState_[scancode];
    }

    InputHandler::InputHandler() : currentState_(), previousState_()
    {
        currentState_.fill(false);
        previousState_.fill(false);
    }
}
