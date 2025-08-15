//
// Created by blomq on 2025-08-11.
//

#ifndef PSYENGINE_INPUT_MANAGER_HPP
#define PSYENGINE_INPUT_MANAGER_HPP

#include "psyengine_export.h"

#include <string>
#include <unordered_map>
#include <variant>

#include <SDL3/SDL.h>

#include "timer.hpp"

namespace psyengine
{
    /**
     * @class InputManager
     * @brief Manages input from the keyboard, mouse, and gamepads and provides an interface for
     *        binding actions to input events and querying their state.
     *
     * The InputManager class facilitates input handling by managing bindings for actions to keys,
     * mouse buttons, and gamepad buttons. It also tracks the states of various input sources and
     * processes SDL events to update them. This class enables querying input states such as whether
     * a button is pressed, held, released, or clicked.
     */
    class InputManager
    {
        template <typename T, typename U>
        using GamePad = std::unordered_map<SDL_JoystickID, std::unordered_map<T, U>>;

    public:
        static InputManager& instance();

        /**
         * @enum MouseButton
         * @brief Represents the different standard buttons on a mouse.
         *
         * The MouseButton enum provides a mapping of mouse buttons to SDL constants.
         * It is used to identify and differentiate between the left, right, middle,
         * and additional buttons (X1 and X2) on a mouse.
         */
        enum MouseButton : Uint8
        {
            Left   = SDL_BUTTON_LEFT,
            Right  = SDL_BUTTON_RIGHT,
            Middle = SDL_BUTTON_MIDDLE,
            X1     = SDL_BUTTON_X1,
            X2     = SDL_BUTTON_X2,
        };

        /**
         * @enum ButtonState
         * @brief Represents the state of an input button or key.
         *
         * The ButtonState enum defines the various states that an input button or key
         * can be in during interaction. It is used to describe the current conditions
         * of a button or key across different input devices, such as gamepads, keyboards,
         * or mice.
         *
         * - Up: The button or key is not pressed.
         * - Down: The button or key was just pressed and is currently held down.
         * - Clicked: The button or key was pressed and released in quick succession.
         * - Held: The button or key has been continuously held down over multiple updates.
         * - Released: The button or key was just released after being pressed.
         */
        enum class ButtonState : Uint8
        {
            Up,
            Down,
            Clicked,
            Held,
            Released
        };

        /**
         * @struct ButtonData
         * @brief Represents the state and timing information of a button input.
         *
         * The ButtonData structure holds the current and previous state of a button, including whether the button
         * is pressed or released. It also records the precise time the button was pressed and its overall state.
         * This is useful for tracking button interactions and their transitions over time.
         */
        struct PSYENGINE_EXPORT ButtonData
        {
            bool isDown = false;
            bool wasDown = false;
            time::TimePoint pressTime{};
            ButtonState state = ButtonState::Up;
        };

        /**
         * @struct AxisData
         * @brief Represents the data for an input axis, including raw value and user-defined scaling.
         *
         * The AxisData structure stores information specific to an input axis, such as the raw SDL-provided
         * value and an optional scaling factor for adjusting sensitivity or range as per user requirements.
         */
        struct PSYENGINE_EXPORT AxisData
        {
            Sint16 value = 0; // raw SDL value (-32 768 to 32 767)
            float scale = 1.0f; // user-configurable scaling if desired
        };

        // --- BINDING TYPES FOR ACTIONS ---
        struct PSYENGINE_EXPORT KeyBinding
        {
            SDL_Keycode key;
            bool operator==(const KeyBinding& other) const = default;
        };

        struct PSYENGINE_EXPORT MouseBinding
        {
            Uint8 button;
            bool operator==(const MouseBinding& other) const = default;
        };

        struct PSYENGINE_EXPORT GamepadBinding
        {
            SDL_GamepadButton button;
            SDL_JoystickID joystickId; // 0 means "any"
            bool operator==(const GamepadBinding& other) const = default;
        };

        using Binding = std::variant<KeyBinding, MouseBinding, GamepadBinding>;

        struct Action
        {
            std::vector<Binding> bindings;
        };

        // --- Action binding API ---

        /**
         * @brief Binds a specific keyboard key to an action name for input management.
         *
         * This method associates a provided SDL_Keycode with a designated action name,
         * enabling the InputManager to track and handle input events related to the specified action.
         *
         * @param actionName The name of the action to bind the key to.
         * @param key The SDL_Keycode representing the key to be bound to the action.
         */
        PSYENGINE_EXPORT void bindActionKey(const std::string& actionName, SDL_Keycode key);
        /**
         * @brief Binds a mouse button to a specified action name.
         *
         * This method associates a mouse button with an action, allowing the action to be triggered
         * when the mouse button is interacted with. The binding is stored internally for input handling.
         *
         * @param actionName The name of the action to bind the mouse button to.
         * @param button The mouse button to be associated with the action.
         */
        PSYENGINE_EXPORT void bindActionMouseButton(const std::string& actionName, Uint8 button);

        /**
         * @brief Binds a specified gamepad button to a named action for a specific joystick.
         *
         * This function associates a gamepad button with a user-defined action name for the given
         * joystick, represented by its ID. When the button is pressed on the specified joystick, it
         * will be associated with the corresponding action for input handling purposes.
         *
         * @param actionName The name of the action to bind to the gamepad button.
         * @param button The gamepad button to bind to the action.
         * @param joystickId The ID of the joystick to which the binding applies. 0 means any
         */
        PSYENGINE_EXPORT void bindActionGamepadButton(const std::string& actionName, SDL_GamepadButton button,
                                     SDL_JoystickID joystickId = 0);

        // --- Queries for actions (aggregated over all bindings) ---

        /**
         * @brief Checks if the specified action has been clicked.
         *
         * Determines whether the action associated with the given action name has been
         * triggered by a click event. Supports input types such as keyboard keys,
         * mouse buttons, or gamepad buttons.
         *
         * @param actionName The name of the action to check for a click event.
         * @return True if the action is clicked, otherwise false.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isActionClicked(const std::string& actionName) const;
        /**
         * @brief Checks if an input action is currently being held down.
         *
         * This method determines if the given action, mapped to a key, mouse button, or gamepad button,
         * is currently in a held state. The function iterates through all the bindings associated with
         * the specified action and returns true if any of them is held.
         *
         * @param actionName The name of the action to check for held state.
         * @return True if the action is being held, otherwise false.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isActionHeld(const std::string& actionName) const;
        /**
         * @brief Checks whether the specified action is currently being performed, based on its binding(s).
         *
         * This method evaluates all bindings associated with the given action name (e.g., keys, mouse buttons,
         * or gamepad inputs) and determines if any of the bindings are currently active. An action is considered
         * "down" if any of its associated input bindings is in the active state.
         *
         * @param actionName The name of the action to check.
         * @return True if any of the bindings for the specified action are active; false otherwise.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isActionDown(const std::string& actionName) const;
        /**
         * @brief Checks if the specified action has been released.
         *
         * This method determines if the given action, which may be bound to a key, mouse button, or
         * gamepad button, is in the released state.
         *
         * @param actionName The name of the action to check for a release state.
         * @return True if the action is released, otherwise false.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isActionReleased(const std::string& actionName) const;

        /**
         * @brief Handles an SDL event and processes it to update input states accordingly.
         *
         * This method interprets various SDL events, such as key presses, mouse clicks, and gamepad inputs,
         * to update the internal state of the input manager. Events related to input devices, such as
         * keyboard, mouse, or gamepad, are used to track button presses, releases, and axis movements.
         *
         * @param e The SDL_Event instance representing the event to be processed.
         */
        PSYENGINE_EXPORT void handleEvent(const SDL_Event& e);

        /**
         * @brief Updates the state of input devices including keyboard, mouse, and gamepads.
         *
         * This method processes and updates the internal state of all input devices by determining the
         * current state (e.g., Up, Down, or Held) of each input button based on their activity since the
         * last update. It evaluates mouse, keyboard, and gamepad input, ensuring up-to-date information
         * about their interaction states.
         *
         * The update logic accounts for transitions between states, such as a button being pressed or
         * released, and updates the respective attributes accordingly. This maintenance allows the InputManager
         * to provide consistent and accurate input state queries for bound actions.
         *
         * @note Has to be called after handling all events and before updating game logic
         */
        PSYENGINE_EXPORT void update();

        /**
         * @brief Checks whether the specified key was clicked (pressed and released) during the current frame.
         *
         * Determines if the given key has transitioned from a pressed state to a released state
         * within the duration of the current frame.
         *
         * @param key The SDL_Keycode representing the key to check for the clicked state.
         * @return true if the specified key was clicked during this frame, false otherwise.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isClicked(SDL_Keycode key) const;

        /**
         * @brief Checks if the specified key is currently being held down.
         *
         * This method determines whether the given key is in the "held" state
         * by comparing its current state to the internal button state tracking.
         *
         * @param key The SDL_Keycode representing the key to check.
         * @return True if the key is currently held down, false otherwise.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isHeld(SDL_Keycode key) const;

        /**
         * @brief Checks if the specified key is currently pressed or held.
         *
         * Determines if the given key is in the "Down" or "Held" state, signifying
         * that it is either being pressed during the current frame or has been held
         * continuously across frames.
         *
         * @param key The SDL_Keycode representing the key to check.
         * @return True if the key is pressed or held, otherwise false.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isDown(SDL_Keycode key) const;

        /**
         * @brief Checks if the specified key has been released.
         *
         * This method determines whether a specified keyboard key was released during the last input event processing cycle.
         *
         * @param key The SDL_Keycode representing the key to check.
         * @return True if the key has been released, false otherwise.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isReleased(SDL_Keycode key) const;

        /**
         * @brief Checks if the specified gamepad button was clicked for the given joystick.
         *
         * A button click is defined as a button that was pressed and then released within a single
         * update cycle.
         *
         * @param button The gamepad button to check.
         * @param joystickId The unique ID of the joystick to which the button belongs.
         * @return True if the button was clicked, false otherwise.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isClicked(SDL_GamepadButton button, SDL_JoystickID joystickId = 0) const;

        /**
         * @brief Checks if a specified gamepad button on a specific joystick is currently being held down.
         *
         * This method determines the state of the given button on the specified joystick and returns
         * whether it is in the "Held" state.
         *
         * @param button The gamepad button to check.
         * @param joystickId The ID of the joystick associated with the gamepad.
         * @return True if the button is held down; otherwise, false.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isHeld(SDL_GamepadButton button, SDL_JoystickID joystickId = 0) const;

        /**
         * @brief Checks if a specific gamepad button is currently pressed or held down.
         *
         * This method determines whether the specified gamepad button on a given gamepad
         * (identified by its joystick ID) is in a "Down" or "Held" state.
         *
         * @param button The gamepad button to check.
         * @param joystickId The ID of the gamepad's joystick to query.
         * @return True if the button is pressed or held down; otherwise, false.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isDown(SDL_GamepadButton button, SDL_JoystickID joystickId = 0) const;

        /**
         * @brief Checks whether the specified gamepad button was released for the given joystick.
         *
         * This function determines if the specified button on the gamepad, associated with the
         * provided joystick identifier, is in the released state.
         *
         * @param button The gamepad button to check.
         * @param joystickId The ID of the joystick to check the button state for.
         * @return True if the specified button is released, false otherwise.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isReleased(SDL_GamepadButton button, SDL_JoystickID joystickId = 0) const;

        /**
         * @brief Checks whether the specified mouse button was clicked.
         *
         * This method verifies if the given mouse button was clicked by evaluating its
         * current state as "Clicked". A click is defined as the transition from a released
         * state to a pressed state and back within a short duration.
         *
         * @param mouseButton The mouse button to check for a click event.
         * @return True if the specified mouse button was clicked; otherwise, false.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isClicked(Uint8 mouseButton) const;

        /**
         * @brief Checks if the specified mouse button is currently held down.
         *
         * This method verifies whether the given mouse button is in a "held" state,
         * which means it is continuously being pressed without being released.
         *
         * @param mouseButton The mouse button to check.
         * @return True if the mouse button is being held down, otherwise false.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isHeld(Uint8 mouseButton) const;

        /**
         * @brief Checks if the specified mouse button is currently pressed or held down.
         *
         * This method determines the state of the given mouse button and returns true
         * if the button is in the "Down" or "Held" state.
         *
         * @param mouseButton The mouse button to query.
         * @return True if the mouse button is pressed or held, otherwise false.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isDown(Uint8 mouseButton) const;

        /**
         * @brief Checks if the specified mouse button is in the released state.
         *
         * This method determines whether the given mouse button has been released during the
         * input processing cycle.
         *
         * @param mouseButton The mouse button to query its release state.
         * @return True if the specified mouse button is released, false otherwise.
         */
        [[nodiscard]] PSYENGINE_EXPORT bool isReleased(Uint8 mouseButton) const;

        /**
         * @brief Retrieves the raw value of a specified gamepad axis for a given joystick.
         *
         * This method returns the current raw value of a gamepad axis for the specified joystick.
         * If the axis or joystick is not found, it returns a default value of 0.
         *
         * @param gamepadAxis The specific gamepad axis to query. This value is of the type SDL_GamepadAxis.
         * @param joystickId The identifier of the joystick to which the axis belongs. This value is of the type SDL_JoystickID.
         * @return The raw value of the specified axis as a signed 16-bit integer (Sint16).
         *         Returns 0 if the axis or joystick does not exist in the mapping.
         */
        [[nodiscard]] PSYENGINE_EXPORT Sint16 getAxisRaw(SDL_GamepadAxis gamepadAxis, SDL_JoystickID joystickId = 0) const;

        /**
         * @brief Retrieves the normalized value of a specific gamepad axis for a given joystick.
         *
         * This method returns a value in the range of -1.0 to 1.0, representing the proportion of the
         * axis's movement relative to its maximum range. The normalization is based on the axis's raw value
         * divided by the maximum positive axis value.
         *
         * @param gamepadAxis The SDL_GamepadAxis representing the gamepad's specific axis (e.g., left stick horizontal, right stick vertical).
         * @param joystickId The SDL_JoystickID associated with the joystick being queried.
         * @return The normalized axis value in the range of -1.0 to 1.0, or 0.0 if the raw axis value is zero.
         */
        [[nodiscard]] PSYENGINE_EXPORT float getAxisNormalized(SDL_GamepadAxis gamepadAxis,
                                              SDL_JoystickID joystickId = 0) const;

        /**
         * @brief Sets the duration threshold for considering an input as a "hold".
         *
         * This function allows setting the time interval required for an input to transition
         * into a "held" state. The threshold is specified in seconds and determines how long
         * an input must be held down before it is recognized as such.
         *
         * @param seconds The hold threshold time in seconds.
         */
        PSYENGINE_EXPORT void setHoldThreshold(float seconds);

        /**
         * @brief Retrieves the threshold duration that determines when an input is considered held.
         *
         * This method returns the configured time duration, in seconds, that an input must be active
         * to be classified as being held. It is used to differentiate between press and hold states
         * for input events.
         *
         * @return The hold threshold duration in seconds.
         */
        [[nodiscard]] PSYENGINE_EXPORT float getHoldThreshold() const;

        InputManager(const InputManager& other) = delete;
        InputManager(InputManager&& other) noexcept = delete;
        InputManager& operator=(const InputManager& other) = delete;
        InputManager& operator=(InputManager&& other) noexcept = delete;

    private:
        PSYENGINE_EXPORT InputManager() :
            holdThreshold_(0.3f)
        {
        }

        PSYENGINE_EXPORT ~InputManager() = default;

        std::unordered_map<Uint8, ButtonData> mouseButtons_;
        std::unordered_map<SDL_Keycode, ButtonData> keyboardButtons_;

        GamePad<SDL_GamepadButton, ButtonData> gamepadButtons_;
        GamePad<SDL_GamepadAxis, AxisData> axes_;

        std::unordered_map<std::string, Action> actions_;

        float holdThreshold_;

        // Helper to check each binding of an action with a callable that returns bool
        template <typename Func>
        bool forEachBinding(const std::string& actionName, Func&& func) const;

        // ---- event handlers ----
        void onButtonPress(SDL_Keycode key, time::TimePoint now);
        void onButtonRelease(SDL_Keycode key);

        void onButtonPress(SDL_GamepadButton gamepadButton, time::TimePoint now,
                           SDL_JoystickID joystickId = 0);
        void onButtonRelease(SDL_GamepadButton gamepadButton,
                             SDL_JoystickID joystickId = 0);

        void onButtonPress(Uint8 mouseButton, time::TimePoint now);
        void onButtonRelease(Uint8 mouseButton);

        // ---- getters ----
        [[nodiscard]] ButtonState getButtonState(SDL_GamepadButton button,
                                                 SDL_JoystickID joystickId = 0) const;
        [[nodiscard]] ButtonState getButtonState(SDL_Keycode key) const;
        [[nodiscard]] ButtonState getButtonState(Uint8 mouseButton) const;

        static std::string getKeyName(SDL_Keycode key);
        static std::string getGamepadButtonName(SDL_GamepadButton button);
        static std::string getGamepadAxisName(SDL_GamepadAxis axis);

        void updateGamepads(const time::TimePoint& now);
        void updateMouseButtons(const time::TimePoint& now);
        void updateKeyboardButtons(const time::TimePoint& now);
    };

#include "input_manager.tpp"

}

#endif //PSYENGINE_INPUT_MANAGER_HPP
