//
// Created by blomq on 2025-08-11.
//

#ifndef PSYENGINE_BASE_STATE_HPP
#define PSYENGINE_BASE_STATE_HPP



// ReSharper disable twice CppInconsistentNaming
union SDL_Event;
struct SDL_Renderer;

namespace psyengine::state
{
    /**
     * @class BaseState
     * @brief A base class defining the interface for game states to implement.
     *
     * This abstract class provides a set of virtual methods that derived classes
     * must or can implement to define their specific behavior. These methods
     * facilitate the actions needed during a game's state transitions and interactions.
     */
    class  BaseState
    {

    public:
        /**
         * Virtual destructor for the BaseState class.
         *
         * Ensures proper cleanup of derived class resources when a BaseState pointer
         * is deleted. This is important for cases where dynamic polymorphism is used
         * and resources need to be released appropriately for derived instances.
         */
        virtual ~BaseState() = default;

        BaseState(const BaseState& other) = delete;
        BaseState& operator=(const BaseState& other) = delete;

        BaseState(BaseState&&) noexcept
        {}

        BaseState& operator=(BaseState&& other) noexcept
        {
            if (this == &other)
            {
                return *this;
            }

            return *this;
        }

    protected:
        /**
         * @class StateManager
         * @brief Manages the stack of game states.
         *
         * Provides functionality to manage the lifecycle and transitions of game states,
         * including pushing, replacing, and popping states. The class ensures proper handling
         * of the states, forwarding events, updates, and rendering operations to the active state.
         */
        friend class StateManager;

        /**
         * Called when the state is entered.
         * Override this method to perform any necessary initialization for the state.
         * By default, it returns true indicating successful initialization.
         *
         * @return True if the state entered successfully, false otherwise.
         */
        virtual bool onEnter()
        {
            return true;
        };

        /**
         * A pure virtual method meant to be overridden by derived classes.
         * This method is invoked when the state is exited.
         * Derived classes should implement this to handle resource cleanup or
         * any actions required when the state transitions out.
         */
        virtual void onExit() = 0;

        /**
         * Handles an incoming SDL_Event. This method is called to process events
         * such as input or other actions affecting the current state. The function
         * must be implemented by derived classes to define specific behavior for
         * handling events.
         *
         * @param event A constant reference to an SDL_Event object that represents
         *              the input event or action to process.
         */
        virtual void handleEvent(const SDL_Event& event) = 0;

        /**
         * Performs a fixed update on the state with a fixed time interval.
         * This method is called at a constant rate, independent of the frame rate.
         * It is primarily used for physics updates or other operations that
         * require consistent timing.
         *
         * @param deltaTime The fixed timestep duration in seconds.
         */
        virtual void fixedUpdate([[maybe_unused]] double deltaTime) = 0;

        /**
         * Updates the state with a time delta.
         *
         * This method is intended to be implemented by derived classes to update the
         * state based on a given time delta. The time delta is typically used to advance
         * logic or animations proportional to the elapsed time.
         *
         * @param deltaTime The time that has elapsed since the last update, expressed in seconds.
         *                  Maybe unused in some implementations.
         */
        virtual void update([[maybe_unused]] double deltaTime) = 0;
        /**
         * Renders the current state using the specified SDL renderer.
         * This method must be implemented by derived classes, and it is called
         * to draw the contents of the state with optional interpolation for smooth rendering.
         *
         * @param renderer The SDL_Renderer used for drawing graphics.
         * @param interpolationFactor The interpolation factor used to smooth rendering between updates.
         *        This parameter is optional and may not be used depending on the implementation.
         */
        virtual void render(SDL_Renderer* renderer, [[maybe_unused]] float interpolationFactor) = 0;
    };
}

#endif //PSYENGINE_BASE_STATE_HPP
