//
// Created by blomq on 2025-08-11.
//

#include "psyengine/state_manager.hpp"

#include "psyengine/base_state.hpp"

namespace psyengine
{
    StateManager& StateManager::instance()
    {
        static StateManager instance;
        return instance;
    }

    void StateManager::handleEvent(SDL_Event* const event) const
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

    void StateManager::render(SDL_Renderer* const renderer) const
    {
        if (states_.empty())
        {
            return;
        }

        states_.back()->render(renderer);
    }

    bool StateManager::pushState(std::unique_ptr<BaseState> state)
    {
        if (!state)
        {
            return false;
        }

        states_.push_back(std::move(state));
        try
        {
            return states_.back()->onEnter();
        }
        catch (...)
        {
            // Roll back to maintain stack consistency if onEnter throws
            states_.pop_back();
            //throw; // or return false; choose based on your error-handling policy
            return false;
        }
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
