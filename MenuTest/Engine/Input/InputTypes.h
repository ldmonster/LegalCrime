#pragma once

#include "../Core/Types.h"
#include <SDL3/SDL.h>

namespace Engine {
namespace Input {

    // Input device types
    enum class InputDevice {
        Keyboard,
        Mouse,
        Gamepad,
        Unknown
    };

    // Mouse buttons
    enum class MouseButton {
        Left = SDL_BUTTON_LEFT,
        Middle = SDL_BUTTON_MIDDLE,
        Right = SDL_BUTTON_RIGHT,
        X1 = SDL_BUTTON_X1,
        X2 = SDL_BUTTON_X2
    };

    // Input state for buttons/keys
    enum class InputState {
        Released,   // Not pressed
        Pressed,    // Just pressed this frame
        Held,       // Held down
        JustReleased // Just released this frame
    };

    // Keyboard modifier keys
    enum class KeyModifier {
        None = 0,
        Shift = 1 << 0,
        Ctrl = 1 << 1,
        Alt = 1 << 2,
        GUI = 1 << 3  // Windows key, Command key
    };

    // Gamepad buttons (maps to SDL_GamepadButton values)
    enum class GamepadButton {
        A = SDL_GAMEPAD_BUTTON_SOUTH,
        B = SDL_GAMEPAD_BUTTON_EAST,
        X = SDL_GAMEPAD_BUTTON_WEST,
        Y = SDL_GAMEPAD_BUTTON_NORTH,
        Back = SDL_GAMEPAD_BUTTON_BACK,
        Guide = SDL_GAMEPAD_BUTTON_GUIDE,
        Start = SDL_GAMEPAD_BUTTON_START,
        LeftStick = SDL_GAMEPAD_BUTTON_LEFT_STICK,
        RightStick = SDL_GAMEPAD_BUTTON_RIGHT_STICK,
        LeftShoulder = SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,
        RightShoulder = SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
        DPadUp = SDL_GAMEPAD_BUTTON_DPAD_UP,
        DPadDown = SDL_GAMEPAD_BUTTON_DPAD_DOWN,
        DPadLeft = SDL_GAMEPAD_BUTTON_DPAD_LEFT,
        DPadRight = SDL_GAMEPAD_BUTTON_DPAD_RIGHT
    };

    // Gamepad axes (maps to SDL_GamepadAxis values)
    enum class GamepadAxis {
        LeftX = SDL_GAMEPAD_AXIS_LEFTX,
        LeftY = SDL_GAMEPAD_AXIS_LEFTY,
        RightX = SDL_GAMEPAD_AXIS_RIGHTX,
        RightY = SDL_GAMEPAD_AXIS_RIGHTY,
        LeftTrigger = SDL_GAMEPAD_AXIS_LEFT_TRIGGER,
        RightTrigger = SDL_GAMEPAD_AXIS_RIGHT_TRIGGER
    };

    // Mouse position
    struct MousePosition {
        int x;
        int y;

        MousePosition() : x(0), y(0) {}
        MousePosition(int x, int y) : x(x), y(y) {}
    };

    // Mouse delta (for relative movement)
    struct MouseDelta {
        int dx;
        int dy;

        MouseDelta() : dx(0), dy(0) {}
        MouseDelta(int dx, int dy) : dx(dx), dy(dy) {}
    };

    // Snapshot of mouse state for UI consumption (no SDL dependency).
    struct MouseState {
        float x;
        float y;
        bool leftDown;
        bool rightDown;
        bool middleDown;

        MouseState() : x(0), y(0), leftDown(false), rightDown(false), middleDown(false) {}
        MouseState(float mx, float my, bool left, bool right = false, bool middle = false)
            : x(mx), y(my), leftDown(left), rightDown(right), middleDown(middle) {}
    };

} // namespace Input
} // namespace Engine
