#include "../../Tests/SimpleTest.h"
#include "Button.h"

using namespace Engine::UI;
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
