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
    class InputManager
    {
        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;

        template <typename T, typename U>
        using GamePad = std::unordered_map<SDL_JoystickID, std::unordered_map<T, U>>;

    public:
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

        explicit InputManager(const float holdThresholdSeconds = 0.3f) :
            holdThreshold_(std::chrono::duration<float>(holdThresholdSeconds))
        {
        }

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

        void handleEvent(const SDL_Event& e);

        // Call this once per frame (after events, before game logic) to compute states
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

    private:
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
        void onButtonRelease(SDL_Keycode key, TimePoint now);

        void onButtonPress(SDL_GamepadButton gamepadButton, TimePoint now,
                           SDL_JoystickID joystickId = 0);
        void onButtonRelease(SDL_GamepadButton gamepadButton, TimePoint now,
                             SDL_JoystickID joystickId = 0);

        void onButtonPress(MouseButton mouseButton, TimePoint now);
        void onButtonRelease(MouseButton mouseButton, TimePoint now);

        // ---- getters ----
        [[nodiscard]] ButtonState getButtonState(SDL_GamepadButton button,
                                                 SDL_JoystickID joystickId = 0) const;
        [[nodiscard]] ButtonState getButtonState(SDL_Keycode key) const;
        [[nodiscard]] ButtonState getButtonState(MouseButton mouseButton) const;

        static std::string getKeyName(SDL_Keycode key);
        static std::string getGamepadButtonName(SDL_GamepadButton button);
        static std::string getGamepadAxisName(SDL_GamepadAxis axis);
    };

    template <typename Func>
    bool InputManager::forEachBinding(const std::string& actionName, Func&& func) const
    {
        if (const auto it = actions_.find(actionName); it != actions_.end())
        {
            for (const auto& binding : it->second.bindings)
            {
                if (func(binding, *this))
                    return true; // any binding is enough to return true
            }
        }
        return false;
    }
}

#endif //PSYENGINE_INPUT_MANAGER_HPP
