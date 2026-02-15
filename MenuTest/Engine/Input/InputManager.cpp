#include "InputManager.h"

namespace Engine {
namespace Input {

    InputManager::InputManager(ILogger* logger)
        : m_logger(logger)
        , m_enabled(true)
        , m_keyboardState(nullptr)
        , m_mouseButtonState(0)
        , m_mouseWheelDelta(0) {
    }

    InputManager::~InputManager() {
        Shutdown();
    }

    void InputManager::Initialize() {
        if (m_logger) {
            m_logger->Info("InputManager initialized");
        }

        // Get initial keyboard state
        m_keyboardState = SDL_GetKeyboardState(nullptr);
    }

    void InputManager::Shutdown() {
        ClearAll();
        
        if (m_logger) {
            m_logger->Info("InputManager shutdown");
        }
    }

    void InputManager::ProcessEvent(const SDL_Event& event) {
        if (!m_enabled) return;

        switch (event.type) {
            case SDL_EVENT_MOUSE_MOTION:
                m_mousePosition.x = static_cast<int>(event.motion.x);
                m_mousePosition.y = static_cast<int>(event.motion.y);
                break;

            case SDL_EVENT_MOUSE_WHEEL:
                m_mouseWheelDelta = static_cast<int>(event.wheel.y);
                break;

            default:
                break;
        }
    }

    void InputManager::Update() {
        if (!m_enabled) {
            ResetFrameState();
            return;
        }

        // Update keyboard state
        m_keyboardState = SDL_GetKeyboardState(nullptr);

        // Update mouse state
        float mouseX, mouseY;
        m_mouseButtonState = SDL_GetMouseState(&mouseX, &mouseY);
        
        // Calculate mouse delta
        m_mouseDelta.dx = m_mousePosition.x - m_previousMousePosition.x;
        m_mouseDelta.dy = m_mousePosition.y - m_previousMousePosition.y;
        m_previousMousePosition = m_mousePosition;

        // Update all actions and axes
        UpdateActions();
        UpdateAxes();

        // Reset per-frame state at the end
        ResetFrameState();
    }

    InputAction* InputManager::CreateAction(const std::string& name) {
        // Check if action already exists
        auto it = m_actions.find(name);
        if (it != m_actions.end()) {
            if (m_logger) {
                m_logger->Warning("Action '" + name + "' already exists");
            }
            return it->second.get();
        }

        // Create new action
        auto action = std::make_unique<InputAction>(name);
        InputAction* actionPtr = action.get();
        m_actions[name] = std::move(action);

        if (m_logger) {
            m_logger->Debug("Created input action: " + name);
        }

        return actionPtr;
    }

    InputAction* InputManager::GetAction(const std::string& name) {
        auto it = m_actions.find(name);
        return (it != m_actions.end()) ? it->second.get() : nullptr;
    }

    void InputManager::RemoveAction(const std::string& name) {
        m_actions.erase(name);
    }

    InputAxis* InputManager::CreateAxis(const std::string& name) {
        // Check if axis already exists
        auto it = m_axes.find(name);
        if (it != m_axes.end()) {
            if (m_logger) {
                m_logger->Warning("Axis '" + name + "' already exists");
            }
            return it->second.get();
        }

        // Create new axis
        auto axis = std::make_unique<InputAxis>(name);
        InputAxis* axisPtr = axis.get();
        m_axes[name] = std::move(axis);

        if (m_logger) {
            m_logger->Debug("Created input axis: " + name);
        }

        return axisPtr;
    }

    InputAxis* InputManager::GetAxis(const std::string& name) {
        auto it = m_axes.find(name);
        return (it != m_axes.end()) ? it->second.get() : nullptr;
    }

    void InputManager::RemoveAxis(const std::string& name) {
        m_axes.erase(name);
    }

    bool InputManager::IsActionPressed(const std::string& name) {
        InputAction* action = GetAction(name);
        return action ? action->IsPressed() : false;
    }

    bool InputManager::IsActionHeld(const std::string& name) {
        InputAction* action = GetAction(name);
        return action ? action->IsHeld() : false;
    }

    bool InputManager::IsActionReleased(const std::string& name) {
        InputAction* action = GetAction(name);
        return action ? action->IsReleased() : false;
    }

    bool InputManager::WasActionJustPressed(const std::string& name) {
        InputAction* action = GetAction(name);
        return action ? action->WasJustPressed() : false;
    }

    bool InputManager::WasActionJustReleased(const std::string& name) {
        InputAction* action = GetAction(name);
        return action ? action->WasJustReleased() : false;
    }

    float InputManager::GetAxisValue(const std::string& name) {
        InputAxis* axis = GetAxis(name);
        return axis ? axis->GetValue() : 0.0f;
    }

    bool InputManager::IsKeyPressed(SDL_Keycode key) {
        if (!m_keyboardState) return false;

        SDL_Scancode scancode = SDL_GetScancodeFromKey(key, nullptr);
        bool isCurrentlyDown = m_keyboardState[scancode];
        bool wasPreviouslyDown = m_previousKeyState[key];

        return isCurrentlyDown && !wasPreviouslyDown;
    }

    bool InputManager::IsKeyHeld(SDL_Keycode key) {
        if (!m_keyboardState) return false;

        SDL_Scancode scancode = SDL_GetScancodeFromKey(key, nullptr);
        return m_keyboardState[scancode];
    }

    bool InputManager::IsMouseButtonPressed(MouseButton button) {
        uint32_t buttonMask = SDL_BUTTON_MASK(static_cast<int>(button));
        return (m_mouseButtonState & buttonMask) != 0;
    }

    bool InputManager::IsMouseButtonHeld(MouseButton button) {
        return IsMouseButtonPressed(button);
    }

    bool InputManager::IsShiftHeld() const {
        SDL_Keymod modState = SDL_GetModState();
        return (modState & SDL_KMOD_SHIFT) != 0;
    }

    bool InputManager::IsCtrlHeld() const {
        SDL_Keymod modState = SDL_GetModState();
        return (modState & SDL_KMOD_CTRL) != 0;
    }

    bool InputManager::IsAltHeld() const {
        SDL_Keymod modState = SDL_GetModState();
        return (modState & SDL_KMOD_ALT) != 0;
    }

    void InputManager::ClearAll() {
        m_actions.clear();
        m_axes.clear();
        m_previousKeyState.clear();
    }

    void InputManager::UpdateActions() {
        // Update previous keyboard state
        if (m_keyboardState) {
            for (auto& pair : m_actions) {
                // Store previous state for edge detection
                for (auto& action : m_actions) {
                    // Actions will update themselves based on current input
                    action.second->UpdateFromKeyboard(m_keyboardState, nullptr);
                    action.second->UpdateFromMouse(m_mouseButtonState, nullptr);
                }
            }
        }
    }

    void InputManager::UpdateAxes() {
        for (auto& pair : m_axes) {
            InputAxis* axis = pair.second.get();
            axis->UpdateFromKeyboard(m_keyboardState);
            // Mouse delta is updated per-axis if bound
        }
    }

    void InputManager::ResetFrameState() {
        // Reset per-frame state
        m_mouseWheelDelta = 0;

        // Reset action frame states
        for (auto& pair : m_actions) {
            pair.second->ResetFrameState();
        }

        // Reset axis frame states
        for (auto& pair : m_axes) {
            pair.second->ResetFrameState();
        }
    }

} // namespace Input
} // namespace Engine
