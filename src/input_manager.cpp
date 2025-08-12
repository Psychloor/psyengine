//
// Created by blomq on 2025-08-11.
//

#include "psyengine/input_manager.hpp"

namespace psyengine
{

    void InputManager::bindActionKey(const std::string& actionName, const SDL_Keycode key)
    {
        actions_[actionName].bindings.emplace_back(KeyBinding{key});
    }

    void InputManager::bindActionMouseButton(const std::string& actionName, const MouseButton button)
    {
        actions_[actionName].bindings.emplace_back(MouseBinding{button});
    }

    void InputManager::bindActionGamepadButton(const std::string& actionName, const SDL_GamepadButton button,
                                               const SDL_JoystickID joystickId)
    {
        actions_[actionName].bindings.emplace_back(GamepadBinding{button, joystickId});
    }

    bool InputManager::isActionClicked(const std::string& actionName) const
    {
        return forEachBinding(actionName, [](const Binding& b, const InputManager& mgr)
        {
            return std::visit([&]<typename TBinding>(TBinding&& bind) -> bool
            {
                using T = std::decay_t<TBinding>;
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

    bool InputManager::isActionHeld(const std::string& actionName) const
    {
        return forEachBinding(actionName, [](const Binding& b, const InputManager& mgr)
        {
            return std::visit([&]<typename TBinding>(TBinding&& bind) -> bool
            {
                using T = std::decay_t<TBinding>;
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

    bool InputManager::isActionDown(const std::string& actionName) const
    {
        return forEachBinding(actionName, [](const Binding& b, const InputManager& mgr)
        {
            return std::visit([&]<typename TBinding>(TBinding&& bind) -> bool
            {
                using T = std::decay_t<TBinding>;
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

    bool InputManager::isActionReleased(const std::string& actionName) const
    {
        return forEachBinding(actionName, [](const Binding& b, const InputManager& mgr)
        {
            return std::visit([&]<typename TBinding>(TBinding&& bind) -> bool
            {
                using T = std::decay_t<TBinding>;
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

    void InputManager::handleEvent(const SDL_Event& e)
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

    void InputManager::update()
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

    bool InputManager::isClicked(const SDL_Keycode key) const
    {
        return getButtonState(key) == ButtonState::Clicked;
    }

    bool InputManager::isHeld(const SDL_Keycode key) const
    {
        return getButtonState(key) == ButtonState::Held;
    }

    bool InputManager::isDown(const SDL_Keycode key) const
    {
        const auto st = getButtonState(key);
        return st == ButtonState::Down || st == ButtonState::Held;
    }

    bool InputManager::isReleased(const SDL_Keycode key) const
    {
        return getButtonState(key) == ButtonState::Released;
    }

    bool InputManager::isClicked(const SDL_GamepadButton button, const SDL_JoystickID joystickId) const
    {
        return getButtonState(button, joystickId) == ButtonState::Clicked;
    }

    bool InputManager::isHeld(const SDL_GamepadButton button, const SDL_JoystickID joystickId) const
    {
        return getButtonState(button, joystickId) == ButtonState::Held;
    }

    bool InputManager::isDown(const SDL_GamepadButton button, const SDL_JoystickID joystickId) const
    {
        const auto st = getButtonState(button, joystickId);
        return st == ButtonState::Down || st == ButtonState::Held;
    }

    bool InputManager::isReleased(const SDL_GamepadButton button, const SDL_JoystickID joystickId) const
    {
        return getButtonState(button, joystickId) == ButtonState::Released;
    }

    bool InputManager::isClicked(const MouseButton mouseButton) const
    {
        return getButtonState(mouseButton) == ButtonState::Clicked;
    }

    bool InputManager::isHeld(const MouseButton mouseButton) const
    {
        return getButtonState(mouseButton) == ButtonState::Held;
    }

    bool InputManager::isDown(const MouseButton mouseButton) const
    {
        const auto st = getButtonState(mouseButton);
        return st == ButtonState::Down || st == ButtonState::Held;
    }

    bool InputManager::isReleased(const MouseButton mouseButton) const
    {
        return getButtonState(mouseButton) == ButtonState::Released;
    }

    Sint16 InputManager::getAxisRaw(const SDL_GamepadAxis gamepadAxis, const SDL_JoystickID joystickId) const
    {
        if (const auto it = axes_.find(joystickId); it != axes_.end())
        {
            if (const auto axesIt = it->second.find(gamepadAxis); axesIt != it->second.end())
                return axesIt->second.value;
        }
        return 0;
    }

    float InputManager::getAxisNormalized(const SDL_GamepadAxis gamepadAxis, const SDL_JoystickID joystickId) const
    {
        // use positive max for division to keep -1..1 roughly symmetric
        static constexpr auto JOYSTICK_MAX = static_cast<float>(SDL_JOYSTICK_AXIS_MAX); // 32767
        const auto raw = getAxisRaw(gamepadAxis, joystickId);
        if (raw == 0)
            return 0.0f;
        return static_cast<float>(raw) / JOYSTICK_MAX;
    }

    void InputManager::onButtonPress(const SDL_Keycode key, const TimePoint now)
    {
        auto& btn = keyboardButtons_[key];
        btn.isDown = true;
        btn.pressTime = now;
    }

    void InputManager::onButtonRelease(const SDL_Keycode key, const TimePoint now)
    {
        auto& btn = keyboardButtons_[key];
        btn.isDown = false;
    }

    void InputManager::onButtonPress(const SDL_GamepadButton gamepadButton, const TimePoint now,
                                     const SDL_JoystickID joystickId)
    {
        auto& btn = gamepadButtons_[joystickId][gamepadButton];
        btn.isDown = true;
        btn.pressTime = now;
    }

    void InputManager::onButtonRelease(const SDL_GamepadButton gamepadButton, const TimePoint now,
                                       const SDL_JoystickID joystickId)
    {
        auto& btn = gamepadButtons_[joystickId][gamepadButton];
        btn.isDown = false;
    }

    void InputManager::onButtonPress(const MouseButton mouseButton, const TimePoint now)
    {
        auto& btn = mouseButtons_[mouseButton];
        btn.isDown = true;
        btn.pressTime = now;
    }

    void InputManager::onButtonRelease(const MouseButton mouseButton, const TimePoint now)
    {
        auto& btn = mouseButtons_[mouseButton];
        btn.isDown = false;
    }

    InputManager::ButtonState InputManager::getButtonState(const SDL_GamepadButton button,
                                                           const SDL_JoystickID joystickId) const
    {
        if (const auto it = gamepadButtons_.find(joystickId); it != gamepadButtons_.end())
        {
            if (const auto buttonIt = it->second.find(button); buttonIt != it->second.end())
                return buttonIt->second.state;
        }
        return ButtonState::Up;
    }

    InputManager::ButtonState InputManager::getButtonState(const SDL_Keycode key) const
    {
        if (const auto it = keyboardButtons_.find(key); it != keyboardButtons_.end())
            return it->second.state;
        return ButtonState::Up;
    }

    InputManager::ButtonState InputManager::getButtonState(const MouseButton mouseButton) const
    {
        if (const auto it = mouseButtons_.find(mouseButton); it != mouseButtons_.end())
            return it->second.state;
        return ButtonState::Up;
    }

    std::string InputManager::getKeyName(const SDL_Keycode key)
    {
        return SDL_GetKeyName(key);
    }

    std::string InputManager::getGamepadButtonName(const SDL_GamepadButton button)
    {
        return {SDL_GetGamepadStringForButton(button)};
    }

    std::string InputManager::getGamepadAxisName(const SDL_GamepadAxis axis)
    {
        return {SDL_GetGamepadStringForAxis(axis)};
    }
}
