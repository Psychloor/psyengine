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

        void bindActionKey(const std::string& actionName, const SDL_Keycode key)
        {
            actions_[actionName].bindings.emplace_back(KeyBinding{key});
        }

        void bindActionMouseButton(const std::string& actionName, const MouseButton button)
        {
            actions_[actionName].bindings.emplace_back(MouseBinding{button});
        }

        // joystickId=0 means "any joystick"
        void bindActionGamepadButton(const std::string& actionName, const SDL_GamepadButton button,
                                     const SDL_JoystickID joystickId = 0)
        {
            actions_[actionName].bindings.emplace_back(GamepadBinding{button, joystickId});
        }

        // --- Queries for actions (aggregated over all bindings) ---

        [[nodiscard]] bool isActionClicked(const std::string& actionName) const
        {
            return forEachBinding(actionName, [](const Binding& b, const InputManager& mgr)
            {
                return std::visit([&]<typename T0>(T0&& bind) -> bool
                {
                    using T = std::decay_t<T0>;
                    if constexpr (std::is_same_v<T, KeyBinding>)
                        return mgr.isClicked(bind.key);
                    else if constexpr (std::is_same_v<T, MouseBinding>)
                        return mgr.isClicked(bind.button);
                    else if constexpr (std::is_same_v<T, GamepadBinding>)
                        return mgr.isClicked(bind.button, bind.joystickId);
                    else
                        return false;
                }, b);
            });
        }

        [[nodiscard]] bool isActionHeld(const std::string& actionName) const
        {
            return forEachBinding(actionName, [](const Binding& b, const InputManager& mgr)
            {
                return std::visit([&]<typename T0>(T0&& bind) -> bool
                {
                    using T = std::decay_t<T0>;
                    if constexpr (std::is_same_v<T, KeyBinding>)
                        return mgr.isHeld(bind.key);
                    else if constexpr (std::is_same_v<T, MouseBinding>)
                        return mgr.isHeld(bind.button);
                    else if constexpr (std::is_same_v<T, GamepadBinding>)
                        return mgr.isHeld(bind.button, bind.joystickId);
                    else
                        return false;
                }, b);
            });
        }

        [[nodiscard]] bool isActionDown(const std::string& actionName) const
        {
            return forEachBinding(actionName, [](const Binding& b, const InputManager& mgr)
            {
                return std::visit([&]<typename T0>(T0&& bind) -> bool
                {
                    using T = std::decay_t<T0>;
                    if constexpr (std::is_same_v<T, KeyBinding>)
                        return mgr.isDown(bind.key);
                    else if constexpr (std::is_same_v<T, MouseBinding>)
                        return mgr.isDown(bind.button);
                    else if constexpr (std::is_same_v<T, GamepadBinding>)
                        return mgr.isDown(bind.button, bind.joystickId);
                    else
                        return false;
                }, b);
            });
        }

        [[nodiscard]] bool isActionReleased(const std::string& actionName) const
        {
            return forEachBinding(actionName, [](const Binding& b, const InputManager& mgr)
            {
                return std::visit([&]<typename T0>(T0&& bind) -> bool
                {
                    using T = std::decay_t<T0>;
                    if constexpr (std::is_same_v<T, KeyBinding>)
                        return mgr.isReleased(bind.key);
                    else if constexpr (std::is_same_v<T, MouseBinding>)
                        return mgr.isReleased(bind.button);
                    else if constexpr (std::is_same_v<T, GamepadBinding>)
                        return mgr.isReleased(bind.button, bind.joystickId);
                    else
                        return false;
                }, b);
            });
        }

        void handleEvent(const SDL_Event& e)
        {
            switch (e.type)
            {
            case SDL_EVENT_KEY_DOWN:
                if (!e.key.repeat)
                    onButtonPress(e.key.key, Clock::now());
                break;
            case SDL_EVENT_KEY_UP:
                onButtonRelease(e.key.key, Clock::now());
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                onButtonPress(static_cast<MouseButton>(e.button.button), Clock::now());
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                onButtonRelease(static_cast<MouseButton>(e.button.button), Clock::now());
                break;

            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                onButtonPress(static_cast<SDL_GamepadButton>(e.gbutton.button), Clock::now(), e.gbutton.which);
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                onButtonRelease(static_cast<SDL_GamepadButton>(e.gbutton.button), Clock::now(), e.gbutton.which);
                break;

            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            {
                auto& axis = axes_[e.gaxis.which][static_cast<SDL_GamepadAxis>(e.gaxis.axis)];
                axis.value = e.gaxis.value; // store raw value
                break;
            }

            case SDL_EVENT_GAMEPAD_REMOVED:
            {
                // remove data for that joystick id to avoid stale entries
                const SDL_JoystickID jid = e.gdevice.which;
                gamepadButtons_.erase(jid);
                axes_.erase(jid);
                break;
            }

            default:
                break;
            }
        }

        // Call this once per frame (after events, before game logic) to compute states
        void update()
        {
            const auto now = Clock::now();

            // mouse
            // ReSharper disable once CppUseElementsView
            for (auto& [_, mouseButton] : mouseButtons_)
            {
                mouseButton.state = ButtonState::Up;

                if (mouseButton.isDown)
                {
                    if (auto heldTime = now - mouseButton.pressTime; heldTime >= holdThreshold_)
                        mouseButton.state = ButtonState::Held;
                    else
                        mouseButton.state = ButtonState::Down;
                }
                else
                {
                    if (mouseButton.wasDown)
                    {
                        if (auto heldTime = now - mouseButton.pressTime; heldTime < holdThreshold_)
                            mouseButton.state = ButtonState::Clicked;
                        else
                            mouseButton.state = ButtonState::Released;
                    }
                }
                mouseButton.wasDown = mouseButton.isDown;
            }

            // keyboard
            // ReSharper disable once CppUseElementsView
            for (auto& [_, keyboardButton] : keyboardButtons_)
            {
                keyboardButton.state = ButtonState::Up;

                if (keyboardButton.isDown)
                {
                    if (auto heldTime = now - keyboardButton.pressTime; heldTime >= holdThreshold_)
                        keyboardButton.state = ButtonState::Held;
                    else
                        keyboardButton.state = ButtonState::Down;
                }
                else
                {
                    if (keyboardButton.wasDown)
                    {
                        if (auto heldTime = now - keyboardButton.pressTime; heldTime < holdThreshold_)
                            keyboardButton.state = ButtonState::Clicked;
                        else
                            keyboardButton.state = ButtonState::Released;
                    }
                }
                keyboardButton.wasDown = keyboardButton.isDown;
            }

            // gamepads (per-joystick)
            // ReSharper disable once CppUseElementsView
            for (auto& [jid, buttonsMap] : gamepadButtons_)
            {
                // ReSharper disable once CppUseElementsView
                for (auto& [btn, gamepadButton] : buttonsMap)
                {
                    gamepadButton.state = ButtonState::Up;

                    if (gamepadButton.isDown)
                    {
                        if (auto heldTime = now - gamepadButton.pressTime; heldTime >= holdThreshold_)
                            gamepadButton.state = ButtonState::Held;
                        else
                            gamepadButton.state = ButtonState::Down;
                    }
                    else
                    {
                        if (gamepadButton.wasDown)
                        {
                            if (auto heldTime = now - gamepadButton.pressTime; heldTime < holdThreshold_)
                                gamepadButton.state = ButtonState::Clicked;
                            else
                                gamepadButton.state = ButtonState::Released;
                        }
                    }
                    gamepadButton.wasDown = gamepadButton.isDown;
                }
            }
        }

        // -------- queries (keyboard) --------
        [[nodiscard]] bool isClicked(const SDL_Keycode key) const
        {
            return getButtonState(key) == ButtonState::Clicked;
        }

        [[nodiscard]] bool isHeld(const SDL_Keycode key) const
        {
            return getButtonState(key) == ButtonState::Held;
        }

        [[nodiscard]] bool isDown(const SDL_Keycode key) const
        {
            const auto st = getButtonState(key);
            return st == ButtonState::Down || st == ButtonState::Held;
        }

        [[nodiscard]] bool isReleased(const SDL_Keycode key) const
        {
            return getButtonState(key) == ButtonState::Released;
        }

        // -------- queries (gamepad) --------
        [[nodiscard]] bool isClicked(const SDL_GamepadButton button, const SDL_JoystickID joystickId = 0) const
        {
            return getButtonState(button, joystickId) == ButtonState::Clicked;
        }

        [[nodiscard]] bool isHeld(const SDL_GamepadButton button, const SDL_JoystickID joystickId = 0) const
        {
            return getButtonState(button, joystickId) == ButtonState::Held;
        }

        [[nodiscard]] bool isDown(const SDL_GamepadButton button, const SDL_JoystickID joystickId = 0) const
        {
            const auto st = getButtonState(button, joystickId);
            return st == ButtonState::Down || st == ButtonState::Held;
        }

        [[nodiscard]] bool isReleased(const SDL_GamepadButton button, const SDL_JoystickID joystickId = 0) const
        {
            return getButtonState(button, joystickId) == ButtonState::Released;
        }

        // -------- queries (mouse) --------
        [[nodiscard]] bool isClicked(const MouseButton mouseButton) const
        {
            return getButtonState(mouseButton) == ButtonState::Clicked;
        }

        [[nodiscard]] bool isHeld(const MouseButton mouseButton) const
        {
            return getButtonState(mouseButton) == ButtonState::Held;
        }

        [[nodiscard]] bool isDown(const MouseButton mouseButton) const
        {
            const auto st = getButtonState(mouseButton);
            return st == ButtonState::Down || st == ButtonState::Held;
        }

        [[nodiscard]] bool isReleased(const MouseButton mouseButton) const
        {
            return getButtonState(mouseButton) == ButtonState::Released;
        }

        // -------- axis raw / normalized --------
        [[nodiscard]] Sint16 getAxisRaw(const SDL_GamepadAxis gamepadAxis, const SDL_JoystickID joystickId = 0) const
        {
            if (const auto it = axes_.find(joystickId); it != axes_.end())
            {
                if (const auto axesIt = it->second.find(gamepadAxis); axesIt != it->second.end())
                    return axesIt->second.value;
            }
            return 0;
        }

        [[nodiscard]] float getAxisNormalized(const SDL_GamepadAxis gamepadAxis,
                                              const SDL_JoystickID joystickId = 0) const
        {
            // use positive max for division to keep -1..1 roughly symmetric
            static constexpr auto JOYSTICK_MAX = static_cast<float>(SDL_JOYSTICK_AXIS_MAX); // 32767
            const auto raw = getAxisRaw(gamepadAxis, joystickId);
            if (raw == 0)
                return 0.0f;
            return static_cast<float>(raw) / JOYSTICK_MAX;
        }

    private:
        std::unordered_map<MouseButton, ButtonData> mouseButtons_;
        std::unordered_map<SDL_Keycode, ButtonData> keyboardButtons_;

        GamePad<SDL_GamepadButton, ButtonData> gamepadButtons_;
        GamePad<SDL_GamepadAxis, AxisData> axes_;

        std::unordered_map<std::string, Action> actions_;

        std::chrono::duration<float> holdThreshold_;

        // Helper to check each binding of an action with a callable that returns bool
        template <typename Func>
        bool forEachBinding(const std::string& actionName, Func&& func) const
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

        // ---- event handlers ----
        void onButtonPress(const SDL_Keycode key, const TimePoint now)
        {
            auto& btn = keyboardButtons_[key];
            btn.isDown = true;
            btn.pressTime = now;
        }

        void onButtonRelease(const SDL_Keycode key, const TimePoint now)
        {
            auto& btn = keyboardButtons_[key];
            btn.isDown = false;
        }

        void onButtonPress(const SDL_GamepadButton gamepadButton, const TimePoint now,
                           const SDL_JoystickID joystickId = 0)
        {
            auto& btn = gamepadButtons_[joystickId][gamepadButton];
            btn.isDown = true;
            btn.pressTime = now;
        }

        void onButtonRelease(const SDL_GamepadButton gamepadButton, const TimePoint now,
                             const SDL_JoystickID joystickId = 0)
        {
            auto& btn = gamepadButtons_[joystickId][gamepadButton];
            btn.isDown = false;
        }

        void onButtonPress(const MouseButton mouseButton, const TimePoint now)
        {
            auto& btn = mouseButtons_[mouseButton];
            btn.isDown = true;
            btn.pressTime = now;
        }

        void onButtonRelease(const MouseButton mouseButton, const TimePoint now)
        {
            auto& btn = mouseButtons_[mouseButton];
            btn.isDown = false;
        }

        // ---- getters ----
        [[nodiscard]] ButtonState getButtonState(const SDL_GamepadButton button,
                                                 const SDL_JoystickID joystickId = 0) const
        {
            if (const auto it = gamepadButtons_.find(joystickId); it != gamepadButtons_.end())
            {
                if (const auto buttonIt = it->second.find(button); buttonIt != it->second.end())
                    return buttonIt->second.state;
            }
            return ButtonState::Up;
        }

        [[nodiscard]] ButtonState getButtonState(const SDL_Keycode key) const
        {
            if (const auto it = keyboardButtons_.find(key); it != keyboardButtons_.end())
                return it->second.state;
            return ButtonState::Up;
        }

        [[nodiscard]] ButtonState getButtonState(const MouseButton mouseButton) const
        {
            if (const auto it = mouseButtons_.find(mouseButton); it != mouseButtons_.end())
                return it->second.state;
            return ButtonState::Up;
        }

        static std::string getKeyName(const SDL_Keycode key)
        {
            return SDL_GetKeyName(key);
        }

        static std::string getGamepadButtonName(const SDL_GamepadButton button)
        {
            return {SDL_GetGamepadStringForButton(button)};
        }

        static std::string getGamepadAxisName(const SDL_GamepadAxis axis)
        {
            return {SDL_GetGamepadStringForAxis(axis)};
        }
    };
}

#endif //PSYENGINE_INPUT_MANAGER_HPP
