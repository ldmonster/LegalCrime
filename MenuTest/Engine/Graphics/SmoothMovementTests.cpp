#include "../../Tests/SimpleTest.h"
#include "AnimatedSprite.h"
#include <cmath>

using namespace Engine;
using namespace SimpleTest;

// ============================================================================
// SmoothMovement Construction
// ============================================================================

TEST_CASE(SmoothMovement_DefaultState_NotMoving) {
    SmoothMovement sm;
    ASSERT_FALSE(sm.isMoving);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SmoothMovement_DefaultProgress_IsOne) {
    SmoothMovement sm;
    ASSERT_FLOAT_NEAR(sm.GetProgress(), 1.0f, 0.001f);
    return TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// SmoothMovement Start
// ============================================================================

TEST_CASE(SmoothMovement_Start_SetsFields) {
    SmoothMovement sm;
    sm.Start(10, 20, 100, 200, 0.5f);

    ASSERT_TRUE(sm.isMoving);
    ASSERT_EQUAL(sm.startX, 10);
    ASSERT_EQUAL(sm.startY, 20);
    ASSERT_EQUAL(sm.targetX, 100);
    ASSERT_EQUAL(sm.targetY, 200);
    ASSERT_FLOAT_NEAR(sm.moveDuration, 0.5f, 0.001f);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SmoothMovement_Start_SamePosition_NoMovement) {
    SmoothMovement sm;
    sm.Start(10, 20, 10, 20, 0.5f);

    ASSERT_FALSE(sm.isMoving);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SmoothMovement_Start_ZeroDuration_UsesDefault) {
    SmoothMovement sm;
    sm.Start(0, 0, 100, 100, 0.0f);

    ASSERT_FLOAT_NEAR(sm.moveDuration, 0.3f, 0.001f);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SmoothMovement_Start_NegativeDuration_UsesDefault) {
    SmoothMovement sm;
    sm.Start(0, 0, 100, 100, -1.0f);

    ASSERT_FLOAT_NEAR(sm.moveDuration, 0.3f, 0.001f);
    return TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// SmoothMovement Update
// ============================================================================

TEST_CASE(SmoothMovement_Update_InterpolatesHalfway) {
    SmoothMovement sm;
    sm.Start(0, 0, 100, 200, 1.0f);

    int x, y;
    bool still = sm.Update(0.5f, x, y);  // halfway

    ASSERT_TRUE(still);
    ASSERT_EQUAL(x, 50);
    ASSERT_EQUAL(y, 100);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SmoothMovement_Update_CompletesOnTime) {
    SmoothMovement sm;
    sm.Start(0, 0, 100, 200, 0.5f);

    int x, y;
    bool still = sm.Update(0.5f, x, y);

    ASSERT_FALSE(still);
    ASSERT_EQUAL(x, 100);
    ASSERT_EQUAL(y, 200);
    ASSERT_FALSE(sm.isMoving);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SmoothMovement_Update_CompletesOnOvershoot) {
    SmoothMovement sm;
    sm.Start(0, 0, 100, 200, 0.5f);

    int x, y;
    bool still = sm.Update(1.0f, x, y);  // overshoot

    ASSERT_FALSE(still);
    ASSERT_EQUAL(x, 100);
    ASSERT_EQUAL(y, 200);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SmoothMovement_Update_NotMoving_ReturnsFalse) {
    SmoothMovement sm;
    int x = 99, y = 99;
    bool still = sm.Update(0.1f, x, y);

    ASSERT_FALSE(still);
    // x, y should be unchanged since Update early-returns
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SmoothMovement_Update_ProgressIncreases) {
    SmoothMovement sm;
    sm.Start(0, 0, 100, 100, 1.0f);

    float p0 = sm.GetProgress();

    int x, y;
    sm.Update(0.25f, x, y);
    float p1 = sm.GetProgress();

    sm.Update(0.25f, x, y);
    float p2 = sm.GetProgress();

    ASSERT_TRUE(p1 > p0);
    ASSERT_TRUE(p2 > p1);
    return TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// SmoothMovement Stop
// ============================================================================

TEST_CASE(SmoothMovement_Stop_ReturnsTarget) {
    SmoothMovement sm;
    sm.Start(0, 0, 100, 200, 1.0f);

    int x, y;
    sm.Update(0.1f, x, y);  // partially moved
    sm.Stop(x, y);

    ASSERT_EQUAL(x, 100);
    ASSERT_EQUAL(y, 200);
    ASSERT_FALSE(sm.isMoving);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SmoothMovement_Stop_ResetsTime) {
    SmoothMovement sm;
    sm.Start(0, 0, 100, 200, 1.0f);

    int x, y;
    sm.Update(0.5f, x, y);
    sm.Stop(x, y);

    ASSERT_FLOAT_NEAR(sm.GetProgress(), 1.0f, 0.001f);
    return TestResult{__FUNCTION__, true, ""};
}
