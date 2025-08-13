//
// Created by blomq on 2025-08-11.
//

#include "psyengine/state_manager.hpp"

#include "psyengine/base_state.hpp"

namespace psyengine
{
    StateManager& StateManager::instance()
    {
        static StateManager inst;
        return inst;
    }

    void StateManager::handleEvent(const SDL_Event& event) const
    {
        if (states_.empty())
        {
            return;
        }

        states_.back()->handleEvent(event);
    }

    void StateManager::fixedUpdate(const double deltaTime) const
    {
        if (states_.empty())
        {
            return;
        }

        states_.back()->fixedUpdate(deltaTime);
    }

    void StateManager::update(const double deltaTime) const
    {
        if (states_.empty())
        {
            return;
        }

        states_.back()->update(deltaTime);
    }

    void StateManager::render(SDL_Renderer* const renderer, const float interpolationFactor) const
    {
        if (states_.empty())
        {
            return;
        }

        states_.back()->render(renderer, interpolationFactor);
    }

    bool StateManager::pushState(std::unique_ptr<BaseState> state)
    {
        if (!state)
        {
            return false;
        }

        states_.push_back(std::move(state));

        const bool success = states_.back()->onEnter();
        if (!success)
        {
            states_.pop_back();
        }

        return success;
    }

    bool StateManager::popState()
    {
        if (states_.empty())
        {
            return false;
        }

        states_.back()->onExit();
        states_.pop_back();
        return true;
    }

    void StateManager::clear()
    {
        while (!states_.empty())
        {
            states_.back()->onExit();
            states_.pop_back();
        }
    }

    bool StateManager::replaceTopState(std::unique_ptr<BaseState> state)
    {
        if (!popState())
        {
            // If there was no state, just push
            return pushState(std::move(state));
        }
        return pushState(std::move(state));
    }
}
