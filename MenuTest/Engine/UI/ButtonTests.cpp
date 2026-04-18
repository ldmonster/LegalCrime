#include "../../Tests/SimpleTest.h"
#include "Button.h"

using namespace Engine::UI;
using namespace Engine::Input;
using namespace SimpleTest;

// ============================================================================
// Button Construction Tests
// ============================================================================

TEST_CASE(Button_DefaultState_IsNormal) {
    Button btn;
    ASSERT_EQUAL(static_cast<int>(btn.GetState()), static_cast<int>(ButtonState::Normal));
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_DefaultEnabled_IsTrue) {
    Button btn;
    ASSERT_TRUE(btn.IsEnabled());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_SetEnabled_False) {
    Button btn;
    btn.SetEnabled(false);
    ASSERT_FALSE(btn.IsEnabled());
    return TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// Button Bounds Tests
// ============================================================================

TEST_CASE(Button_SetBounds_StoresCorrectly) {
    Button btn;
    Engine::Rect r(10, 20, 100, 50);
    btn.SetBounds(r);

    const Engine::Rect& b = btn.GetBounds();
    ASSERT_EQUAL(b.x, 10);
    ASSERT_EQUAL(b.y, 20);
    ASSERT_EQUAL(b.w, 100);
    ASSERT_EQUAL(b.h, 50);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_IsPointInside_WithinBounds) {
    Button btn;
    btn.SetBounds(Engine::Rect(0, 0, 100, 50));

    ASSERT_TRUE(btn.IsPointInside(50, 25));
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_IsPointInside_OutsideBounds) {
    Button btn;
    btn.SetBounds(Engine::Rect(0, 0, 100, 50));

    ASSERT_FALSE(btn.IsPointInside(150, 25));
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_IsPointInside_AtOrigin) {
    Button btn;
    btn.SetBounds(Engine::Rect(0, 0, 100, 50));

    ASSERT_TRUE(btn.IsPointInside(0, 0));
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_IsPointInside_AtEdge) {
    Button btn;
    btn.SetBounds(Engine::Rect(0, 0, 100, 50));

    // Just at the edge — x < bounds.x + bounds.w
    ASSERT_TRUE(btn.IsPointInside(99, 49));
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_IsPointInside_JustOutside) {
    Button btn;
    btn.SetBounds(Engine::Rect(0, 0, 100, 50));

    ASSERT_FALSE(btn.IsPointInside(100, 50));
    return TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// Button Callback Tests
// ============================================================================

TEST_CASE(Button_SetOnClick_CallbackStored) {
    Button btn;
    bool clicked = false;
    btn.SetOnClick([&clicked]() { clicked = true; });

    // We can't easily trigger the click without SDL events,
    // but verify the callback was stored (no crash on set)
    ASSERT_FALSE(clicked);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_SetOnHover_CallbackStored) {
    Button btn;
    bool hovered = false;
    btn.SetOnHover([&hovered]() { hovered = true; });

    ASSERT_FALSE(hovered);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_RenderWithNullRenderer_NoCrash) {
    Button btn;
    btn.SetBounds(Engine::Rect(0, 0, 100, 50));
    btn.Render(nullptr);

    // Should not crash
    ASSERT_TRUE(true);
    return TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// HandleInput Tests (DIP-compliant mouse state injection)
// ============================================================================

TEST_CASE(Button_HandleInput_MouseOutside_StateNormal) {
    Button btn;
    btn.SetBounds(Engine::Rect(10, 10, 100, 50));

    MouseState mouse(200, 200, false);
    btn.HandleInput(mouse);

    ASSERT_EQUAL(static_cast<int>(btn.GetState()), static_cast<int>(ButtonState::Normal));
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_HandleInput_MouseInside_StateHovered) {
    Button btn;
    btn.SetBounds(Engine::Rect(10, 10, 100, 50));

    MouseState mouse(50, 30, false);
    btn.HandleInput(mouse);

    ASSERT_EQUAL(static_cast<int>(btn.GetState()), static_cast<int>(ButtonState::Hovered));
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_HandleInput_MouseInsideLeftDown_StatePressed) {
    Button btn;
    btn.SetBounds(Engine::Rect(10, 10, 100, 50));

    MouseState mouse(50, 30, true);
    btn.HandleInput(mouse);

    ASSERT_EQUAL(static_cast<int>(btn.GetState()), static_cast<int>(ButtonState::Pressed));
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_HandleInput_ClickDetection) {
    Button btn;
    btn.SetBounds(Engine::Rect(10, 10, 100, 50));

    bool clicked = false;
    btn.SetOnClick([&clicked]() { clicked = true; });

    // Press inside
    MouseState mouseDown(50, 30, true);
    btn.HandleInput(mouseDown);
    ASSERT_FALSE(clicked);

    // Release inside → should trigger click
    MouseState mouseUp(50, 30, false);
    btn.HandleInput(mouseUp);
    ASSERT_TRUE(clicked);

    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_HandleInput_ClickNotFiredOnMoveOutWhilePressed) {
    Button btn;
    btn.SetBounds(Engine::Rect(10, 10, 100, 50));

    bool clicked = false;
    btn.SetOnClick([&clicked]() { clicked = true; });

    // Press inside
    MouseState mouseDown(50, 30, true);
    btn.HandleInput(mouseDown);

    // Move outside while still pressed
    MouseState mouseOutDown(200, 200, true);
    btn.HandleInput(mouseOutDown);

    // Release outside
    MouseState mouseOutUp(200, 200, false);
    btn.HandleInput(mouseOutUp);

    ASSERT_FALSE(clicked);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_HandleInput_Disabled_StaysDisabled) {
    Button btn;
    btn.SetBounds(Engine::Rect(10, 10, 100, 50));
    btn.SetEnabled(false);

    MouseState mouse(50, 30, true);
    btn.HandleInput(mouse);

    ASSERT_EQUAL(static_cast<int>(btn.GetState()), static_cast<int>(ButtonState::Disabled));
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_HandleInput_HoverCallback_OnlyOnce) {
    Button btn;
    btn.SetBounds(Engine::Rect(10, 10, 100, 50));

    int hoverCount = 0;
    btn.SetOnHover([&hoverCount]() { hoverCount++; });

    MouseState mouse(50, 30, false);
    btn.HandleInput(mouse);
    btn.HandleInput(mouse);
    btn.HandleInput(mouse);

    ASSERT_EQUAL(hoverCount, 1);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Button_HandleInput_PressCallback_OnTransition) {
    Button btn;
    btn.SetBounds(Engine::Rect(10, 10, 100, 50));

    int pressCount = 0;
    btn.SetOnPress([&pressCount]() { pressCount++; });

    // First press
    MouseState mouseDown(50, 30, true);
    btn.HandleInput(mouseDown);
    ASSERT_EQUAL(pressCount, 1);

    // Held — should not fire again
    btn.HandleInput(mouseDown);
    ASSERT_EQUAL(pressCount, 1);

    return TestResult{__FUNCTION__, true, ""};
}
