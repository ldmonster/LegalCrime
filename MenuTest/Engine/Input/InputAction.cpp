#include "InputAction.h"
#include <algorithm>

namespace Engine {
namespace Input {

    InputAction::InputAction(const std::string& name)
        : m_name(name)
        , m_state(InputState::Released)
        , m_wasDown(false) {
    }

    void InputAction::BindKey(SDL_Keycode key) {
        // Only add if not already bound
        if (std::find(m_boundKeys.begin(), m_boundKeys.end(), key) == m_boundKeys.end()) {
            m_boundKeys.push_back(key);
        }
    }

    void InputAction::BindMouseButton(MouseButton button) {
        if (std::find(m_boundMouseButtons.begin(), m_boundMouseButtons.end(), button) == m_boundMouseButtons.end()) {
            m_boundMouseButtons.push_back(button);
        }
    }

    void InputAction::BindGamepadButton(GamepadButton button) {
        if (std::find(m_boundGamepadButtons.begin(), m_boundGamepadButtons.end(), button) == m_boundGamepadButtons.end()) {
            m_boundGamepadButtons.push_back(button);
        }
    }

    void InputAction::UnbindKey(SDL_Keycode key) {
        m_boundKeys.erase(
            std::remove(m_boundKeys.begin(), m_boundKeys.end(), key),
            m_boundKeys.end()
        );
    }

    void InputAction::UnbindMouseButton(MouseButton button) {
        m_boundMouseButtons.erase(
            std::remove(m_boundMouseButtons.begin(), m_boundMouseButtons.end(), button),
            m_boundMouseButtons.end()
        );
    }

    void InputAction::UnbindAll() {
        m_boundKeys.clear();
        m_boundMouseButtons.clear();
        m_boundGamepadButtons.clear();
    }

    void InputAction::UpdateFromKeyboard(const bool* keyState, const SDL_Event* event) {
        if (!keyState || m_boundKeys.empty()) return;

        // Check if any bound key is pressed
        bool isDown = false;
        for (SDL_Keycode key : m_boundKeys) {
            SDL_Scancode scancode = SDL_GetScancodeFromKey(key, nullptr);
            if (keyState[scancode]) {
                isDown = true;
                break;
            }
        }

        // Update state based on transitions
        if (isDown && !m_wasDown) {
            m_state = InputState::Pressed;
        } else if (isDown && m_wasDown) {
            m_state = InputState::Held;
        } else if (!isDown && m_wasDown) {
            m_state = InputState::JustReleased;
        } else {
            m_state = InputState::Released;
        }

        m_wasDown = isDown;
    }

    void InputAction::UpdateFromMouse(uint32_t mouseState, const SDL_Event* event) {
        if (m_boundMouseButtons.empty()) return;

        // Check if any bound mouse button is pressed
        bool isDown = false;
        for (MouseButton button : m_boundMouseButtons) {
            uint32_t buttonMask = SDL_BUTTON_MASK(static_cast<int>(button));
            if (mouseState & buttonMask) {
                isDown = true;
                break;
            }
        }

        // Update state based on transitions
        if (isDown && !m_wasDown) {
            m_state = InputState::Pressed;
        } else if (isDown && m_wasDown) {
            m_state = InputState::Held;
        } else if (!isDown && m_wasDown) {
            m_state = InputState::JustReleased;
        } else {
            m_state = InputState::Released;
        }

        m_wasDown = isDown;
    }

    void InputAction::UpdateFromGamepad(/* gamepad state */) {
        // TODO: Implement gamepad support
    }

    void InputAction::ResetFrameState() {
        // Convert "just pressed/released" states to held/released
        if (m_state == InputState::Pressed) {
            m_state = InputState::Held;
        } else if (m_state == InputState::JustReleased) {
            m_state = InputState::Released;
        }
    }

} // namespace Input
} // namespace Engine
