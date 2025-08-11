//
// Created by blomq on 2025-08-11.
//

#ifndef PSYENGINE_BASE_STATE_HPP
#define PSYENGINE_BASE_STATE_HPP

#include "SDL3/SDL_render.h"

namespace psyengine
{
    class BaseState
    {
    public:
        virtual ~BaseState() = default;

        virtual bool onEnter() { return true; };
        virtual void onExit() = 0;

        virtual void handleEvent(SDL_Event* event) = 0;
        virtual void fixedUpdate([[maybe_unused]] double deltaTime) = 0;
        virtual void update([[maybe_unused]] double deltaTime) = 0;
        virtual void render(SDL_Renderer* renderer, [[maybe_unused]] float alpha) = 0;
    };
}

#endif //PSYENGINE_BASE_STATE_HPP
