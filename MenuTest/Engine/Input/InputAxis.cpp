#include "InputAxis.h"
#include <algorithm>
#include <cmath>

namespace Engine {
namespace Input {

    InputAxis::InputAxis(const std::string& name)
        : m_name(name)
        , m_value(0.0f)
        , m_rawValue(0.0f)
        , m_negativeKey(SDLK_UNKNOWN)
        , m_positiveKey(SDLK_UNKNOWN)
        , m_hasKeyBinding(false)
        , m_hasMouseBinding(false)
        , m_mouseIsXAxis(true)
        , m_hasGamepadBinding(false)
        , m_gamepadAxis(GamepadAxis::LeftX)
        , m_sensitivity(1.0f)
        , m_deadZone(0.1f)
        , m_inverted(false) {
    }

    void InputAxis::BindKeys(SDL_Keycode negativeKey, SDL_Keycode positiveKey) {
        m_negativeKey = negativeKey;
        m_positiveKey = positiveKey;
        m_hasKeyBinding = true;
    }

    void InputAxis::BindMouseAxis(bool isXAxis) {
        m_hasMouseBinding = true;
        m_mouseIsXAxis = isXAxis;
    }

    void InputAxis::BindGamepadAxis(GamepadAxis axis) {
        m_hasGamepadBinding = true;
        m_gamepadAxis = axis;
    }

    void InputAxis::UpdateFromKeyboard(const bool* keyState) {
        if (!m_hasKeyBinding || !keyState) {
            return;
        }

        float value = 0.0f;

        SDL_Scancode negativeScancode = SDL_GetScancodeFromKey(m_negativeKey, nullptr);
        SDL_Scancode positiveScancode = SDL_GetScancodeFromKey(m_positiveKey, nullptr);

        if (keyState[negativeScancode]) {
            value -= 1.0f;
        }
        if (keyState[positiveScancode]) {
            value += 1.0f;
        }

        m_rawValue = value;
        m_value = ApplySettings(value);
    }

    void InputAxis::UpdateFromMouse(int mouseDelta) {
        if (!m_hasMouseBinding) {
            return;
        }

        // Convert mouse delta to normalized value
        // Mouse movement is relative, so we just pass through with sensitivity
        float value = static_cast<float>(mouseDelta);
        
        m_rawValue = value;
        m_value = ApplySettings(value);
    }

    void InputAxis::UpdateFromGamepad(/* gamepad state */) {
        // TODO: Implement gamepad support
    }

    void InputAxis::ResetFrameState() {
        // For mouse axes, reset to zero each frame (relative input)
        if (m_hasMouseBinding) {
            m_rawValue = 0.0f;
            m_value = 0.0f;
        }
    }

    float InputAxis::ApplySettings(float rawValue) const {
        float result = rawValue;

        // Apply dead zone
        if (std::abs(result) < m_deadZone) {
            result = 0.0f;
        }

        // Apply sensitivity
        result *= m_sensitivity;

        // Apply inversion
        if (m_inverted) {
            result = -result;
        }

        // Clamp to [-1, 1]
        result = std::max(-1.0f, std::min(1.0f, result));

        return result;
    }

} // namespace Input
} // namespace Engine
