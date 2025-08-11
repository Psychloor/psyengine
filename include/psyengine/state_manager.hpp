//
// Created by blomq on 2025-08-11.
//

#ifndef ROGUELIKE_STATE_MANAGER_HPP
#define ROGUELIKE_STATE_MANAGER_HPP

#include <memory>
#include <vector>

#include "base_state.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_render.h"

namespace psyengine
{
    class StateManager
    {
    public:
        static StateManager& getInstance();

        void handleEvent(SDL_Event* event) const;
        void fixedUpdate(double deltaTime) const;
        void update(double deltaTime) const;
        void render(SDL_Renderer* renderer) const;

        // Returns true if the state was pushed and onEnter() completed successfully
        bool pushState(std::unique_ptr<BaseState> state);
        // Returns true if we popped a state
        bool popState();
        // Calls onExit() on all and clears the stack
        void clear();

        // Replaces the top state
        bool replaceTopState(std::unique_ptr<BaseState> state);

        [[nodiscard]] bool empty() const noexcept { return states_.empty(); }
        [[nodiscard]] BaseState* current() const noexcept { return states_.empty() ? nullptr : states_.back().get(); }

        StateManager(const StateManager&) = delete;
        StateManager& operator=(const StateManager&) = delete;

        StateManager(StateManager&&) = delete;
        StateManager& operator=(StateManager&&) = delete;

    private:
        StateManager() = default;
        ~StateManager() = default;

        std::vector<std::unique_ptr<BaseState>> states_;
    };
}

#endif //ROGUELIKE_STATE_MANAGER_HPP
