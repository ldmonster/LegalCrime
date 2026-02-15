#pragma once

#include "InputTypes.h"
#include <string>
#include <SDL3/SDL.h>

namespace Engine {
namespace Input {

    // Represents a continuous input axis (e.g., Movement, Look)
    // Combines positive and negative keys into a single value [-1, 1]
    class InputAxis {
    public:
        InputAxis(const std::string& name);
        ~InputAxis() = default;

        const std::string& GetName() const { return m_name; }

        // Bind input sources
        void BindKeys(SDL_Keycode negativeKey, SDL_Keycode positiveKey);
        void BindMouseAxis(bool isXAxis); // true = X axis, false = Y axis
        void BindGamepadAxis(GamepadAxis axis);

        // Axis settings
        void SetSensitivity(float sensitivity) { m_sensitivity = sensitivity; }
        void SetDeadZone(float deadZone) { m_deadZone = deadZone; }
        void SetInverted(bool inverted) { m_inverted = inverted; }

        float GetSensitivity() const { return m_sensitivity; }
        float GetDeadZone() const { return m_deadZone; }
        bool IsInverted() const { return m_inverted; }

        // Get axis value [-1, 1]
        float GetValue() const { return m_value; }
        float GetRawValue() const { return m_rawValue; }

        // Internal update methods
        void UpdateFromKeyboard(const bool* keyState);  // SDL3 uses bool*
        void UpdateFromMouse(int mouseDelta);
        void UpdateFromGamepad(/* gamepad state */);
        void ResetFrameState();

    private:
        std::string m_name;
        float m_value;      // Processed value with sensitivity/deadzone
        float m_rawValue;   // Raw input value

        // Keyboard bindings
        SDL_Keycode m_negativeKey;
        SDL_Keycode m_positiveKey;
        bool m_hasKeyBinding;

        // Mouse bindings
        bool m_hasMouseBinding;
        bool m_mouseIsXAxis;

        // Gamepad bindings
        bool m_hasGamepadBinding;
        GamepadAxis m_gamepadAxis;

        // Settings
        float m_sensitivity;
        float m_deadZone;
        bool m_inverted;

        float ApplySettings(float rawValue) const;
    };

} // namespace Input
} // namespace Engine
