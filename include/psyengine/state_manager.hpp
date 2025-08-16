//
// Created by blomq on 2025-08-11.
//

#ifndef PSYENGINE_STATE_MANAGER_HPP
#define PSYENGINE_STATE_MANAGER_HPP

#include "psyengine_export.h"

#include <memory>
#include <vector>

#include "base_state.hpp"

namespace psyengine
{

    /**
     * Manages a stack of states, allowing for adding, removing, and manipulating states.
     * Provides functionality for state transitions and stack management in an application.
     */
    class StateManager
    {
    public:
        /**
         * Provides access to the singleton instance of StateManager.
         * Ensures a single, globally accessible instance throughout the application lifecycle.
         */
        PSYENGINE_EXPORT static StateManager& instance();

        /**
         * Handles an event by delegating it to the top-most state in the stack.
         * If no states are present in the stack, this method will return without processing the event.
         *
         * @param event The SDL_Event to be handled. The event can represent different types of user
         * input or system-level events such as key presses, mouse movement, or device connectivity changes.
         */
        PSYENGINE_EXPORT void handleEvent(const SDL_Event& event) const;

        /**
         * Calls the `fixedUpdate` method on the top state in the stack of states
         * if any states exist. If the state stack is empty, the method returns
         * immediately without performing any operation.
         *
         * @param deltaTime The fixed time step (in seconds) used for updating
         *                  the current state's logic.
         */
        PSYENGINE_EXPORT void fixedUpdate(double deltaTime) const;

        /**
         * Updates the current state or context based on the implemented functionality.
         *
         * @param deltaTime Time elapsed since the last update, usually used to control the update frequency or calculations.
         */
        PSYENGINE_EXPORT void update(double deltaTime) const;


        /**
         * Renders the current state by delegating the rendering operation with the given renderer and interpolation factor.
         *
         * @param renderer The SDL renderer to be used for drawing operations.
         * @param interpolationFactor A float value used to determine the interpolation between frames for smooth rendering.
         */
        PSYENGINE_EXPORT void render(SDL_Renderer* renderer, float interpolationFactor) const;

        /**
         * Pushes a new state onto the state stack. The state is moved into the stack,
         * and its `onEnter` method is called. If the `onEnter` method returns false,
         * the state is removed from the stack and the operation is considered
         * unsuccessful.
         *
         * @param state A unique pointer to the state to be pushed onto the stack.
         *              The ownership of the state is transferred during this operation.
         * @return True if the state was successfully pushed onto the stack and its
         *         `onEnter` method returned true; otherwise, false.
         */
        PSYENGINE_EXPORT bool pushState(std::unique_ptr<BaseState> state);

        /**
         * Removes the current top state from the stack and calls its exit handler.
         *
         * If the state stack is empty, the method does nothing and returns false.
         * Otherwise, it invokes the `onExit` function of the state at the top of the stack,
         * removes it, and returns true.
         *
         * @return true if a state was successfully removed, false if the stack was empty.
         */
        PSYENGINE_EXPORT bool popState();

        /**
         * Clears all elements or states from the current context, leaving it empty.
         **/
        PSYENGINE_EXPORT void clear();

        /**
         * Replaces the top state in the StateManager with the given state. If no state exists,
         * the given state is pushed onto the state stack.
         *
         * @param state The unique pointer to the new state that will replace the top state.
         * @return True if the operation was successful, false otherwise.
         */
        PSYENGINE_EXPORT bool replaceTopState(std::unique_ptr<BaseState> state);

        /**
         * Checks whether the internal state stack is empty.
         *
         * @return true if the state stack is empty, false otherwise.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool empty() const noexcept
        {
            return states_.empty();
        }

        /**
         * Retrieves the current state from the state stack without removing it.
         * This allows inspection of the top state.
         *
         * @return The current state at the top of the stack.
         **/
        [[nodiscard]] PSYENGINE_EXPORT BaseState* current() const noexcept
        {
            return states_.empty() ? nullptr : states_.back().get();
        }

        /**
         * Copy constructor is deleted to prevent copying of the StateManager instance.
         *
         * @param other The StateManager instance to copy from. (deleted)
         * @return This method does not return as it is deleted.
         */
        StateManager(const StateManager& other) = delete;


        /**
         * Assignment operator is deleted to prevent copying of StateManager objects.
         *
         * @return This operator does not return a value as it is deleted.
         */
        StateManager& operator=(const StateManager&) = delete;

        /**
         * Deleted move constructor for StateManager.
         *
         * @return None, as this operation is not allowed.
         */
        StateManager(StateManager&&) = delete;

        /**
         * Deleted move assignment operator to prevent moving of the StateManager instance.
         * Ensures that the StateManager cannot be assigned via move semantics.
         *
         * @return A reference to the current StateManager instance, if the operation were not deleted.
         */
        StateManager& operator=(StateManager&&) = delete;

    private:
        PSYENGINE_EXPORT StateManager() = default;
        PSYENGINE_EXPORT ~StateManager() = default;

        std::vector<std::unique_ptr<BaseState>> states_;
    };
}

#endif //PSYENGINE_STATE_MANAGER_HPP
