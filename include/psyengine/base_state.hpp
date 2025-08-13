//
// Created by blomq on 2025-08-11.
//

#ifndef PSYENGINE_BASE_STATE_HPP
#define PSYENGINE_BASE_STATE_HPP

// ReSharper disable twice CppInconsistentNaming
union SDL_Event;
struct SDL_Renderer;

namespace psyengine
{
    class BaseState
    {
    public:
        virtual ~BaseState() = default;

    protected:
        friend class StateManager;

        virtual bool onEnter() { return true; };
        virtual void onExit() = 0;

        virtual void handleEvent(const SDL_Event& event) = 0;
        virtual void fixedUpdate([[maybe_unused]] double deltaTime) = 0;
        virtual void update([[maybe_unused]] double deltaTime) = 0;
        virtual void render(SDL_Renderer* renderer, [[maybe_unused]] float alpha) = 0;
    };
}

#endif //PSYENGINE_BASE_STATE_HPP
