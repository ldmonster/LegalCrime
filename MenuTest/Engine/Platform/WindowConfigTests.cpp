#include "../../Tests/SimpleTest.h"
#include "../Core/Types.h"

using namespace Engine;
using namespace SimpleTest;

#define PASS return TestResult{__FUNCTION__, true, ""}

// ============================================================================
// DisplayScale Tests
// ============================================================================

TEST_CASE(DisplayScale_DefaultIsOne) {
    DisplayScale scale;
    ASSERT_FLOAT_NEAR(scale.horizontal, 1.0f, 0.001f);
    ASSERT_FLOAT_NEAR(scale.vertical, 1.0f, 0.001f);
    PASS;
}

TEST_CASE(DisplayScale_IsHighDPI_FalseAtDefault) {
    DisplayScale scale;
    ASSERT_FALSE(scale.IsHighDPI());
    PASS;
}

TEST_CASE(DisplayScale_IsHighDPI_TrueWhenScaled) {
    DisplayScale scale(2.0f, 2.0f);
    ASSERT_TRUE(scale.IsHighDPI());
    PASS;
}

TEST_CASE(DisplayScale_IsHighDPI_TrueWhenOnlyHorizontal) {
    DisplayScale scale(1.5f, 1.0f);
    ASSERT_TRUE(scale.IsHighDPI());
    PASS;
}

TEST_CASE(DisplayScale_CustomValues) {
    DisplayScale scale(1.5f, 2.0f);
    ASSERT_FLOAT_NEAR(scale.horizontal, 1.5f, 0.001f);
    ASSERT_FLOAT_NEAR(scale.vertical, 2.0f, 0.001f);
    PASS;
}

// ============================================================================
// WindowConfig High-DPI Tests
// ============================================================================

TEST_CASE(WindowConfig_DefaultHighDPI_IsTrue) {
    WindowConfig config;
    ASSERT_TRUE(config.highDPI);
    PASS;
}
