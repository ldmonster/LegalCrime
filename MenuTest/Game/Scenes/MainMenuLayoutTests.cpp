#include "MainMenuScene.h"
#include "../../Tests/SimpleTest.h"

TEST_CASE(MainMenuLayout_DefaultResolution_MatchesBaseline) {
    auto layout = LegalCrime::MainMenuScene::ComputeButtonLayout(800, 600, 0);
    ASSERT_EQUAL(layout.x, 257);
    ASSERT_EQUAL(layout.y, 152);
    ASSERT_EQUAL(layout.w, 283);
    ASSERT_EQUAL(layout.h, 42);
    return {"MainMenuLayout_DefaultResolution_MatchesBaseline", true, ""};
}

TEST_CASE(MainMenuLayout_ScalesWithResolution) {
    auto layout = LegalCrime::MainMenuScene::ComputeButtonLayout(1600, 1200, 1);
    ASSERT_EQUAL(layout.x, 514);
    ASSERT_TRUE(layout.y > 300);
    ASSERT_EQUAL(layout.w, 566);
    ASSERT_EQUAL(layout.h, 84);
    return {"MainMenuLayout_ScalesWithResolution", true, ""};
}

TEST_CASE(MainMenuLayout_ExitButtonAdjustmentApplied) {
    auto regular = LegalCrime::MainMenuScene::ComputeButtonLayout(800, 600, 4);
    auto exitBtn = LegalCrime::MainMenuScene::ComputeButtonLayout(800, 600, 5);

    // Exit has small upward adjustment so spacing to previous is slightly tighter.
    ASSERT_TRUE(exitBtn.y - regular.y < (42 + 26));
    return {"MainMenuLayout_ExitButtonAdjustmentApplied", true, ""};
}
