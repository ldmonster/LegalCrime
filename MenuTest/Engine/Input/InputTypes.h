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

    // Gamepad buttons (Xbox-style layout)
    enum class GamepadButton {
        A,
        B,
        X,
        Y,
        LeftShoulder,
        RightShoulder,
        Back,
        Start,
        Guide,
        LeftStick,
        RightStick,
        DPadUp,
        DPadDown,
        DPadLeft,
        DPadRight
    };

    // Gamepad axes
    enum class GamepadAxis {
        LeftX,
        LeftY,
        RightX,
        RightY,
        LeftTrigger,
        RightTrigger
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

} // namespace Input
} // namespace Engine
