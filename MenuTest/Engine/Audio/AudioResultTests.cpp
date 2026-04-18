#include "../../Tests/SimpleTest.h"
#include "../Core/Types.h"
#include "../Core/Constants.h"

// =============================================================================
// Result<void> Tests — verifies the error handling pattern used by audio system
// =============================================================================

TEST_CASE(ResultVoid_Success_IsTrue) {
    auto result = Engine::Result<void>::Success();
    ASSERT_TRUE(result);
    ASSERT_TRUE(result.success);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(ResultVoid_Failure_IsFalse) {
    auto result = Engine::Result<void>::Failure("something went wrong");
    ASSERT_FALSE(result);
    ASSERT_FALSE(result.success);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(ResultVoid_Failure_PreservesErrorMessage) {
    auto result = Engine::Result<void>::Failure("Music player not initialized");
    ASSERT_STRING_EQUAL(result.error, "Music player not initialized");
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(ResultVoid_Success_HasEmptyError) {
    auto result = Engine::Result<void>::Success();
    ASSERT_STRING_EQUAL(result.error, "");
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(ResultT_Success_HasValue) {
    auto result = Engine::Result<int>::Success(42);
    ASSERT_TRUE(result);
    ASSERT_EQUAL(result.value, 42);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(ResultT_Failure_HasDefaultValue) {
    auto result = Engine::Result<int>::Failure("error");
    ASSERT_FALSE(result);
    ASSERT_EQUAL(result.value, 0);
    ASSERT_STRING_EQUAL(result.error, "error");
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

// =============================================================================
// Constants Tests — verifies engine constants have expected values
// =============================================================================

TEST_CASE(Constants_Camera_ZoomRange) {
    ASSERT_TRUE(Engine::Constants::Camera::MIN_ZOOM > 0.0f);
    ASSERT_TRUE(Engine::Constants::Camera::MAX_ZOOM > Engine::Constants::Camera::MIN_ZOOM);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Constants_Camera_SmoothnessRange) {
    ASSERT_TRUE(Engine::Constants::Camera::MIN_FOLLOW_SMOOTHNESS > 0.0f);
    ASSERT_TRUE(Engine::Constants::Camera::MAX_FOLLOW_SMOOTHNESS >= Engine::Constants::Camera::MIN_FOLLOW_SMOOTHNESS);
    ASSERT_TRUE(Engine::Constants::Camera::DEFAULT_FOLLOW_SMOOTHNESS >= Engine::Constants::Camera::MIN_FOLLOW_SMOOTHNESS);
    ASSERT_TRUE(Engine::Constants::Camera::DEFAULT_FOLLOW_SMOOTHNESS <= Engine::Constants::Camera::MAX_FOLLOW_SMOOTHNESS);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Constants_Camera_ShakeFrequencies) {
    ASSERT_TRUE(Engine::Constants::Camera::SHAKE_FREQUENCY_X > 0.0f);
    ASSERT_TRUE(Engine::Constants::Camera::SHAKE_FREQUENCY_Y > 0.0f);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Constants_Pathfinding_DiagonalCostValid) {
    // Diagonal cost should be approximately sqrt(2)
    ASSERT_FLOAT_NEAR(Engine::Constants::Pathfinding::DIAGONAL_COST, 1.414f, 0.001f);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Constants_Pathfinding_PoolCapacityPositive) {
    ASSERT_TRUE(Engine::Constants::Pathfinding::DEFAULT_POOL_CAPACITY > 0);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Constants_Window_DefaultDimensions) {
    ASSERT_EQUAL(Engine::Constants::Window::DEFAULT_WIDTH, 800);
    ASSERT_EQUAL(Engine::Constants::Window::DEFAULT_HEIGHT, 600);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}
