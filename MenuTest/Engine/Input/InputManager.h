#pragma once

#include "InputTypes.h"
#include "InputAction.h"
#include "InputAxis.h"
#include "../Core/Logger/ILogger.h"
#include <SDL3/SDL.h>
#include <memory>
#include <unordered_map>
#include <string>

namespace Engine {
namespace Input {

    /// <summary>
    /// Central input management system.
    /// Handles keyboard, mouse, and gamepad input.
    /// Provides action and axis abstractions for game code.
    /// </summary>
    class InputManager {
    public:
        InputManager(ILogger* logger = nullptr);
        ~InputManager();

        // Initialization
        void Initialize();
        void Shutdown();

        // Update (call once per frame, before game logic)
        void ProcessEvent(const SDL_Event& event);
        void Update(); // Call after all events processed

        // Action management
        InputAction* CreateAction(const std::string& name);
        InputAction* GetAction(const std::string& name);
        void RemoveAction(const std::string& name);

        // Axis management
        InputAxis* CreateAxis(const std::string& name);
        InputAxis* GetAxis(const std::string& name);
        void RemoveAxis(const std::string& name);

        // Quick access methods
        bool IsActionPressed(const std::string& name);
        bool IsActionHeld(const std::string& name);
        bool IsActionReleased(const std::string& name);
        bool WasActionJustPressed(const std::string& name);
        bool WasActionJustReleased(const std::string& name);

        float GetAxisValue(const std::string& name);

        // Direct input queries (for non-action based input)
        bool IsKeyPressed(SDL_Keycode key);
        bool IsKeyHeld(SDL_Keycode key);
        bool IsMouseButtonPressed(MouseButton button);
        bool IsMouseButtonHeld(MouseButton button);

        // Mouse state
        MousePosition GetMousePosition() const { return m_mousePosition; }
        MouseDelta GetMouseDelta() const { return m_mouseDelta; }
        int GetMouseWheelDelta() const { return m_mouseWheelDelta; }

        // Keyboard modifiers
        bool IsShiftHeld() const;
        bool IsCtrlHeld() const;
        bool IsAltHeld() const;

        // Enable/disable input
        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

        // Clear all bindings and actions
        void ClearAll();

    private:
        ILogger* m_logger;
        bool m_enabled;

        // Actions and axes
        std::unordered_map<std::string, std::unique_ptr<InputAction>> m_actions;
        std::unordered_map<std::string, std::unique_ptr<InputAxis>> m_axes;

        // Current input state
        const bool* m_keyboardState;  // SDL3 uses bool* instead of uint8_t*
        uint32_t m_mouseButtonState;
        MousePosition m_mousePosition;
        MouseDelta m_mouseDelta;
        int m_mouseWheelDelta;

        // Previous frame state (for edge detection)
        MousePosition m_previousMousePosition;

        // Keyboard state tracking for edge detection
        std::unordered_map<SDL_Keycode, bool> m_previousKeyState;

        void UpdateActions();
        void UpdateAxes();
        void ResetFrameState();
    };

} // namespace Input
} // namespace Engine
