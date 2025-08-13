//
// Created by blomq on 2025-08-11.
//

#ifndef PSYENGINE_INPUT_MANAGER_HPP
#define PSYENGINE_INPUT_MANAGER_HPP

#include <chrono>
#include <string>
#include <unordered_map>
#include <variant>

#include <SDL3/SDL.h>

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
        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;

        template <typename T, typename U>
        using GamePad = std::unordered_map<SDL_JoystickID, std::unordered_map<T, U>>;

    public:
        static InputManager& instance();

        enum MouseButton : Uint8
        {
            Left   = SDL_BUTTON_LEFT,
            Right  = SDL_BUTTON_RIGHT,
            Middle = SDL_BUTTON_MIDDLE,
            X1     = SDL_BUTTON_X1,
            X2     = SDL_BUTTON_X2,
        };

        enum class ButtonState
        {
            Up,
            Down,
            Clicked,
            Held,
            Released
        };

        struct ButtonData
        {
            bool isDown = false;
            bool wasDown = false;
            TimePoint pressTime{};
            ButtonState state = ButtonState::Up;
        };

        struct AxisData
        {
            Sint16 value = 0; // raw SDL value (-32 768 to 32 767)
            float scale = 1.0f; // user-configurable scaling if desired
        };

        // --- BINDING TYPES FOR ACTIONS ---
        struct KeyBinding
        {
            SDL_Keycode key;
            bool operator==(const KeyBinding& other) const = default;
        };

        struct MouseBinding
        {
            MouseButton button;
            bool operator==(const MouseBinding& other) const = default;
        };

        struct GamepadBinding
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

        void bindActionKey(const std::string& actionName, SDL_Keycode key);
        void bindActionMouseButton(const std::string& actionName, MouseButton button);

        // joystickId=0 means "any joystick"
        void bindActionGamepadButton(const std::string& actionName, SDL_GamepadButton button,
                                     SDL_JoystickID joystickId = 0);

        // --- Queries for actions (aggregated over all bindings) ---

        [[nodiscard]] bool isActionClicked(const std::string& actionName) const;
        [[nodiscard]] bool isActionHeld(const std::string& actionName) const;
        [[nodiscard]] bool isActionDown(const std::string& actionName) const;
        [[nodiscard]] bool isActionReleased(const std::string& actionName) const;

        /**
         * @brief Handles an SDL event and processes it to update input states accordingly.
         *
         * This method interprets various SDL events, such as key presses, mouse clicks, and gamepad inputs,
         * to update the internal state of the input manager. Events related to input devices, such as
         * keyboard, mouse, or gamepad, are used to track button presses, releases, and axis movements.
         *
         * @param e The SDL_Event instance representing the event to be processed.
         */
        void handleEvent(const SDL_Event& e);

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
        void update();

        // -------- queries (keyboard) --------
        [[nodiscard]] bool isClicked(SDL_Keycode key) const;
        [[nodiscard]] bool isHeld(SDL_Keycode key) const;
        [[nodiscard]] bool isDown(SDL_Keycode key) const;
        [[nodiscard]] bool isReleased(SDL_Keycode key) const;

        // -------- queries (gamepad) --------
        [[nodiscard]] bool isClicked(SDL_GamepadButton button, SDL_JoystickID joystickId = 0) const;
        [[nodiscard]] bool isHeld(SDL_GamepadButton button, SDL_JoystickID joystickId = 0) const;
        [[nodiscard]] bool isDown(SDL_GamepadButton button, SDL_JoystickID joystickId = 0) const;
        [[nodiscard]] bool isReleased(SDL_GamepadButton button, SDL_JoystickID joystickId = 0) const;

        // -------- queries (mouse) --------
        [[nodiscard]] bool isClicked(MouseButton mouseButton) const;
        [[nodiscard]] bool isHeld(MouseButton mouseButton) const;
        [[nodiscard]] bool isDown(MouseButton mouseButton) const;
        [[nodiscard]] bool isReleased(MouseButton mouseButton) const;

        // -------- axis raw / normalized --------
        [[nodiscard]] Sint16 getAxisRaw(SDL_GamepadAxis gamepadAxis, SDL_JoystickID joystickId = 0) const;
        [[nodiscard]] float getAxisNormalized(SDL_GamepadAxis gamepadAxis,
                                              SDL_JoystickID joystickId = 0) const;

        void setHoldThreshold(float seconds);
        [[nodiscard]] float getHoldThreshold() const;

        InputManager(const InputManager& other) = delete;
        InputManager(InputManager&& other) noexcept = delete;
        InputManager& operator=(const InputManager& other) = delete;
        InputManager& operator=(InputManager&& other) noexcept = delete;

    private:
        InputManager() :
            holdThreshold_(std::chrono::milliseconds(300))
        {
        }

        ~InputManager() = default;

        std::unordered_map<MouseButton, ButtonData> mouseButtons_;
        std::unordered_map<SDL_Keycode, ButtonData> keyboardButtons_;

        GamePad<SDL_GamepadButton, ButtonData> gamepadButtons_;
        GamePad<SDL_GamepadAxis, AxisData> axes_;

        std::unordered_map<std::string, Action> actions_;

        std::chrono::duration<float> holdThreshold_;

        // Helper to check each binding of an action with a callable that returns bool
        template <typename Func>
        bool forEachBinding(const std::string& actionName, Func&& func) const;

        // ---- event handlers ----
        void onButtonPress(SDL_Keycode key, TimePoint now);
        void onButtonRelease(SDL_Keycode key);

        void onButtonPress(SDL_GamepadButton gamepadButton, TimePoint now,
                           SDL_JoystickID joystickId = 0);
        void onButtonRelease(SDL_GamepadButton gamepadButton,
                             SDL_JoystickID joystickId = 0);

        void onButtonPress(MouseButton mouseButton, TimePoint now);
        void onButtonRelease(MouseButton mouseButton);

        // ---- getters ----
        [[nodiscard]] ButtonState getButtonState(SDL_GamepadButton button,
                                                 SDL_JoystickID joystickId = 0) const;
        [[nodiscard]] ButtonState getButtonState(SDL_Keycode key) const;
        [[nodiscard]] ButtonState getButtonState(MouseButton mouseButton) const;

        static std::string getKeyName(SDL_Keycode key);
        static std::string getGamepadButtonName(SDL_GamepadButton button);
        static std::string getGamepadAxisName(SDL_GamepadAxis axis);
    };

#include "input_manager.tpp"

}

#endif //PSYENGINE_INPUT_MANAGER_HPP
