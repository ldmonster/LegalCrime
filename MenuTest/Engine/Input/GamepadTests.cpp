#include "../../Tests/SimpleTest.h"
#include "InputAction.h"
#include "InputAxis.h"
#include "InputTypes.h"

using namespace Engine::Input;
using namespace SimpleTest;

#define PASS return TestResult{__FUNCTION__, true, ""}

// ============================================================================
// Gamepad Button Binding Tests
// ============================================================================

TEST_CASE(InputAction_BindGamepadButton_NoGamepad_StaysReleased) {
    InputAction action("test");
    action.BindGamepadButton(GamepadButton::A);

    // Update with null gamepad — should stay Released
    action.UpdateFromGamepad(nullptr);

    ASSERT_TRUE(action.IsReleased());
    ASSERT_FALSE(action.IsPressed());
    PASS;
}

TEST_CASE(InputAction_BindGamepadButton_NoBindings_NoEffect) {
    InputAction action("test");
    // No gamepad buttons bound, update with null
    action.UpdateFromGamepad(nullptr);
    ASSERT_TRUE(action.IsReleased());
    PASS;
}

TEST_CASE(InputAction_UnbindAll_ClearsGamepadButtons) {
    InputAction action("test");
    action.BindGamepadButton(GamepadButton::A);
    action.BindGamepadButton(GamepadButton::B);
    action.UnbindAll();

    // After unbind, even if gamepad was connected, nothing should be bound
    action.UpdateFromGamepad(nullptr);
    ASSERT_TRUE(action.IsReleased());
    PASS;
}

// ============================================================================
// Gamepad Axis Binding Tests
// ============================================================================

TEST_CASE(InputAxis_BindGamepadAxis_NoGamepad_StaysZero) {
    InputAxis axis("horizontal");
    axis.BindGamepadAxis(GamepadAxis::LeftX);

    // Update with null gamepad — should stay 0
    axis.UpdateFromGamepad(nullptr);

    ASSERT_FLOAT_NEAR(axis.GetValue(), 0.0f, 0.001f);
    PASS;
}

TEST_CASE(InputAxis_BindGamepadAxis_HasBinding) {
    InputAxis axis("horizontal");
    axis.BindGamepadAxis(GamepadAxis::LeftX);

    // Without gamepad, value stays at keyboard default
    axis.UpdateFromGamepad(nullptr);
    ASSERT_FLOAT_NEAR(axis.GetRawValue(), 0.0f, 0.001f);
    PASS;
}

// ============================================================================
// GamepadButton Enum Value Tests
// ============================================================================

TEST_CASE(GamepadButton_A_MapsToSDL) {
    // Verify our enum values match SDL3 gamepad button constants
    ASSERT_EQUAL(static_cast<int>(GamepadButton::A), static_cast<int>(SDL_GAMEPAD_BUTTON_SOUTH));
    ASSERT_EQUAL(static_cast<int>(GamepadButton::B), static_cast<int>(SDL_GAMEPAD_BUTTON_EAST));
    ASSERT_EQUAL(static_cast<int>(GamepadButton::X), static_cast<int>(SDL_GAMEPAD_BUTTON_WEST));
    ASSERT_EQUAL(static_cast<int>(GamepadButton::Y), static_cast<int>(SDL_GAMEPAD_BUTTON_NORTH));
    PASS;
}

TEST_CASE(GamepadAxis_LeftX_MapsToSDL) {
    ASSERT_EQUAL(static_cast<int>(GamepadAxis::LeftX), static_cast<int>(SDL_GAMEPAD_AXIS_LEFTX));
    ASSERT_EQUAL(static_cast<int>(GamepadAxis::RightX), static_cast<int>(SDL_GAMEPAD_AXIS_RIGHTX));
    ASSERT_EQUAL(static_cast<int>(GamepadAxis::LeftTrigger), static_cast<int>(SDL_GAMEPAD_AXIS_LEFT_TRIGGER));
    PASS;
}
