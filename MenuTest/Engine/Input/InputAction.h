#pragma once

#include "InputTypes.h"
#include <string>
#include <vector>
#include <SDL3/SDL.h>

namespace Engine {
namespace Input {

    // Represents a discrete input action (e.g., Jump, Attack, Select)
    class InputAction {
    public:
        InputAction(const std::string& name);
        ~InputAction() = default;

        const std::string& GetName() const { return m_name; }

        // Bind input sources
        void BindKey(SDL_Keycode key);
        void BindMouseButton(MouseButton button);
        void BindGamepadButton(GamepadButton button);

        // Unbind
        void UnbindKey(SDL_Keycode key);
        void UnbindMouseButton(MouseButton button);
        void UnbindAll();

        // Query action state
        bool IsPressed() const { return m_state == InputState::Pressed; }
        bool IsHeld() const { return m_state == InputState::Held || m_state == InputState::Pressed; }
        bool IsReleased() const { return m_state == InputState::Released; }
        bool WasJustPressed() const { return m_state == InputState::Pressed; }
        bool WasJustReleased() const { return m_state == InputState::JustReleased; }

        InputState GetState() const { return m_state; }

        // Internal update methods (called by InputManager)
        void UpdateFromKeyboard(const bool* keyState, const SDL_Event* event);  // SDL3 uses bool*
        void UpdateFromMouse(uint32_t mouseState, const SDL_Event* event);
        void UpdateFromGamepad(/* gamepad state */);
        void ResetFrameState(); // Called at end of frame

    private:
        std::string m_name;
        InputState m_state;
        
        // Bound inputs
        std::vector<SDL_Keycode> m_boundKeys;
        std::vector<MouseButton> m_boundMouseButtons;
        std::vector<GamepadButton> m_boundGamepadButtons;

        bool m_wasDown; // Previous frame state for edge detection
    };

} // namespace Input
} // namespace Engine
