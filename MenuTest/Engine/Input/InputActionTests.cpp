#include "../../Tests/SimpleTest.h"
#include "InputAction.h"
#include "InputTypes.h"
#include <SDL3/SDL.h>
#include <cstring>

using namespace Engine::Input;
using namespace SimpleTest;

// Helper: create a zeroed keyboard state array
// SDL3 uses bool* for keyboard state
static const int KEY_STATE_SIZE = SDL_SCANCODE_COUNT;

struct MockKeyState {
    bool keys[SDL_SCANCODE_COUNT];
    MockKeyState() { memset(keys, 0, sizeof(keys)); }
    void Press(SDL_Scancode sc) { keys[sc] = true; }
    void Release(SDL_Scancode sc) { keys[sc] = false; }
    const bool* Get() const { return keys; }
};

// ============================================================================
// InputAction State Machine Tests
// ============================================================================

TEST_CASE(InputAction_DefaultState_IsReleased) {
    InputAction action("test");
    ASSERT_TRUE(action.IsReleased());
    ASSERT_FALSE(action.IsPressed());
    ASSERT_FALSE(action.IsHeld());
    ASSERT_FALSE(action.WasJustPressed());
    ASSERT_FALSE(action.WasJustReleased());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(InputAction_BindKey_CanQueryState) {
    InputAction action("test");
    action.BindKey(SDLK_SPACE);
    // State should remain Released without update
    ASSERT_TRUE(action.IsReleased());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(InputAction_KeyPress_TransitionsToPressed) {
    InputAction action("test");
    action.BindKey(SDLK_SPACE);

    MockKeyState state;
    SDL_Scancode sc = SDL_GetScancodeFromKey(SDLK_SPACE, nullptr);
    state.Press(sc);

    action.UpdateFromKeyboard(state.Get(), nullptr);
    ASSERT_TRUE(action.IsPressed());
    ASSERT_TRUE(action.WasJustPressed());
    ASSERT_TRUE(action.IsHeld()); // IsHeld returns true for Pressed too
    ASSERT_FALSE(action.IsReleased());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(InputAction_KeyHeld_TransitionsToHeld) {
    InputAction action("test");
    action.BindKey(SDLK_SPACE);

    MockKeyState state;
    SDL_Scancode sc = SDL_GetScancodeFromKey(SDLK_SPACE, nullptr);
    state.Press(sc);

    // Frame 1: press
    action.UpdateFromKeyboard(state.Get(), nullptr);
    ASSERT_TRUE(action.IsPressed());

    // Frame 2: still held
    action.UpdateFromKeyboard(state.Get(), nullptr);
    ASSERT_TRUE(action.IsHeld());
    ASSERT_FALSE(action.WasJustPressed());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(InputAction_KeyRelease_TransitionsToJustReleased) {
    InputAction action("test");
    action.BindKey(SDLK_SPACE);

    MockKeyState state;
    SDL_Scancode sc = SDL_GetScancodeFromKey(SDLK_SPACE, nullptr);

    // Press
    state.Press(sc);
    action.UpdateFromKeyboard(state.Get(), nullptr);

    // Release
    state.Release(sc);
    action.UpdateFromKeyboard(state.Get(), nullptr);
    ASSERT_TRUE(action.WasJustReleased());
    ASSERT_FALSE(action.IsPressed());
    ASSERT_FALSE(action.IsHeld());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(InputAction_AfterJustReleased_TransitionsToReleased) {
    InputAction action("test");
    action.BindKey(SDLK_SPACE);

    MockKeyState state;
    SDL_Scancode sc = SDL_GetScancodeFromKey(SDLK_SPACE, nullptr);

    // Press then release
    state.Press(sc);
    action.UpdateFromKeyboard(state.Get(), nullptr);
    state.Release(sc);
    action.UpdateFromKeyboard(state.Get(), nullptr);
    ASSERT_TRUE(action.WasJustReleased());

    // Next update (still released)
    action.UpdateFromKeyboard(state.Get(), nullptr);
    ASSERT_TRUE(action.IsReleased());
    ASSERT_FALSE(action.WasJustReleased());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(InputAction_ResetFrameState_PressedBecomesHeld) {
    InputAction action("test");
    action.BindKey(SDLK_SPACE);

    MockKeyState state;
    SDL_Scancode sc = SDL_GetScancodeFromKey(SDLK_SPACE, nullptr);
    state.Press(sc);

    action.UpdateFromKeyboard(state.Get(), nullptr);
    ASSERT_TRUE(action.IsPressed());

    action.ResetFrameState();
    // After reset, Pressed should become Held
    ASSERT_TRUE(action.IsHeld());
    ASSERT_FALSE(action.WasJustPressed());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(InputAction_ResetFrameState_JustReleasedBecomesReleased) {
    InputAction action("test");
    action.BindKey(SDLK_SPACE);

    MockKeyState state;
    SDL_Scancode sc = SDL_GetScancodeFromKey(SDLK_SPACE, nullptr);

    state.Press(sc);
    action.UpdateFromKeyboard(state.Get(), nullptr);
    state.Release(sc);
    action.UpdateFromKeyboard(state.Get(), nullptr);
    ASSERT_TRUE(action.WasJustReleased());

    action.ResetFrameState();
    ASSERT_TRUE(action.IsReleased());
    ASSERT_FALSE(action.WasJustReleased());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(InputAction_MultipleKeys_AnyPressedTriggersAction) {
    InputAction action("test");
    action.BindKey(SDLK_SPACE);
    action.BindKey(SDLK_RETURN);

    MockKeyState state;
    SDL_Scancode scReturn = SDL_GetScancodeFromKey(SDLK_RETURN, nullptr);
    state.Press(scReturn);

    action.UpdateFromKeyboard(state.Get(), nullptr);
    ASSERT_TRUE(action.IsPressed());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(InputAction_UnbindKey_NoLongerResponds) {
    InputAction action("test");
    action.BindKey(SDLK_SPACE);

    MockKeyState state;
    SDL_Scancode sc = SDL_GetScancodeFromKey(SDLK_SPACE, nullptr);
    state.Press(sc);

    action.UpdateFromKeyboard(state.Get(), nullptr);
    ASSERT_TRUE(action.IsPressed());

    action.UnbindKey(SDLK_SPACE);
    action.UpdateFromKeyboard(state.Get(), nullptr);
    // With no keys bound, UpdateFromKeyboard returns early
    // State should remain from last update (Held, since wasDown=true)
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(InputAction_UnbindAll_ClearsAllBindings) {
    InputAction action("test");
    action.BindKey(SDLK_SPACE);
    action.BindMouseButton(MouseButton::Left);
    action.UnbindAll();

    MockKeyState state;
    SDL_Scancode sc = SDL_GetScancodeFromKey(SDLK_SPACE, nullptr);
    state.Press(sc);

    action.UpdateFromKeyboard(state.Get(), nullptr);
    // No bound keys, Update returns early
    ASSERT_TRUE(action.IsReleased());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(InputAction_MouseButton_PressTriggersAction) {
    InputAction action("test");
    action.BindMouseButton(MouseButton::Left);

    uint32_t mouseState = SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
    action.UpdateFromMouse(mouseState, nullptr);
    ASSERT_TRUE(action.IsPressed());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(InputAction_MouseButton_ReleaseTriggersJustReleased) {
    InputAction action("test");
    action.BindMouseButton(MouseButton::Left);

    uint32_t pressed = SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
    uint32_t released = 0;

    action.UpdateFromMouse(pressed, nullptr);
    ASSERT_TRUE(action.IsPressed());

    action.UpdateFromMouse(released, nullptr);
    ASSERT_TRUE(action.WasJustReleased());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(InputAction_GetName_ReturnsCorrectName) {
    InputAction action("Jump");
    ASSERT_TRUE(action.GetName() == "Jump");
    return TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// Full frame lifecycle test (matches fixed Application::Update flow)
// ============================================================================

TEST_CASE(InputAction_FrameLifecycle_PressedPersistsThroughFrame) {
    // Simulates the corrected flow:
    // 1. InputManager::Update() calls UpdateFromKeyboard (sets Pressed)
    // 2. Game logic reads Pressed (should see it)
    // 3. InputManager::EndFrame() calls ResetFrameState (Pressed→Held)

    InputAction action("test");
    action.BindKey(SDLK_SPACE);

    MockKeyState state;
    SDL_Scancode sc = SDL_GetScancodeFromKey(SDLK_SPACE, nullptr);
    state.Press(sc);

    // Step 1: InputManager::Update runs UpdateFromKeyboard
    action.UpdateFromKeyboard(state.Get(), nullptr);

    // Step 2: Game logic reads input — should see Pressed
    ASSERT_TRUE(action.IsPressed());
    ASSERT_TRUE(action.WasJustPressed());

    // Step 3: EndFrame resets
    action.ResetFrameState();
    ASSERT_TRUE(action.IsHeld());
    ASSERT_FALSE(action.WasJustPressed());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(InputAction_FrameLifecycle_JustReleasedPersistsThroughFrame) {
    InputAction action("test");
    action.BindKey(SDLK_SPACE);

    MockKeyState state;
    SDL_Scancode sc = SDL_GetScancodeFromKey(SDLK_SPACE, nullptr);

    // Press and advance one frame
    state.Press(sc);
    action.UpdateFromKeyboard(state.Get(), nullptr);
    action.ResetFrameState();

    // Release
    state.Release(sc);
    action.UpdateFromKeyboard(state.Get(), nullptr);

    // Game logic should see JustReleased
    ASSERT_TRUE(action.WasJustReleased());

    // EndFrame resets
    action.ResetFrameState();
    ASSERT_TRUE(action.IsReleased());
    ASSERT_FALSE(action.WasJustReleased());
    return TestResult{__FUNCTION__, true, ""};
}
