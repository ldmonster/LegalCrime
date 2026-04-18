#include "../Tests/SimpleTest.h"
#include "GameConstants.h"

// =============================================================================
// GameConstants Tests — verifies game-layer constants have expected values
// =============================================================================

TEST_CASE(GameConstants_Movement_DurationPositive) {
    ASSERT_TRUE(LegalCrime::Constants::Movement::DEFAULT_MOVE_DURATION > 0.0f);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(GameConstants_Steering_RadiusPositive) {
    ASSERT_TRUE(LegalCrime::Constants::Steering::DEFAULT_SEPARATION_RADIUS > 0.0f);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(GameConstants_Steering_StrengthPositive) {
    ASSERT_TRUE(LegalCrime::Constants::Steering::DEFAULT_SEPARATION_STRENGTH > 0.0f);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(GameConstants_Steering_ThresholdsOrdered) {
    // PUSH_THRESHOLD should be less than MIN_DISTANCE
    ASSERT_TRUE(LegalCrime::Constants::Steering::PUSH_THRESHOLD < LegalCrime::Constants::Steering::MIN_DISTANCE);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(GameConstants_MainMenu_ButtonDimensions) {
    ASSERT_TRUE(LegalCrime::Constants::MainMenu::BUTTON_WIDTH > 0);
    ASSERT_TRUE(LegalCrime::Constants::MainMenu::BUTTON_HEIGHT > 0);
    ASSERT_TRUE(LegalCrime::Constants::MainMenu::BUTTON_RENDER_WIDTH > 0);
    ASSERT_TRUE(LegalCrime::Constants::MainMenu::BUTTON_RENDER_HEIGHT > 0);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(GameConstants_MainMenu_RenderFitsButton) {
    // The render rect should be smaller than the source button sprite
    ASSERT_TRUE(LegalCrime::Constants::MainMenu::BUTTON_RENDER_WIDTH < LegalCrime::Constants::MainMenu::BUTTON_WIDTH);
    ASSERT_TRUE(LegalCrime::Constants::MainMenu::BUTTON_RENDER_HEIGHT < LegalCrime::Constants::MainMenu::BUTTON_HEIGHT);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}
