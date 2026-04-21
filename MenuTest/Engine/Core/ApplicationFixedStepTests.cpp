#include "Application.h"
#include "../../Tests/SimpleTest.h"

TEST_CASE(ApplicationFixedStep_NoTime_NoUpdates) {
    float accumulator = 0.0f;
    int updates = Engine::Application::ComputeFixedUpdateCount(0.0f, 1.0f / 60.0f, 0.1f, 5, accumulator);
    ASSERT_EQUAL(updates, 0);
    ASSERT_FLOAT_NEAR(accumulator, 0.0f, 0.0001f);
    return {"ApplicationFixedStep_NoTime_NoUpdates", true, ""};
}

TEST_CASE(ApplicationFixedStep_SubStepTime_NoUpdateYet) {
    float accumulator = 0.0f;
    int updates = Engine::Application::ComputeFixedUpdateCount(0.005f, 1.0f / 60.0f, 0.1f, 5, accumulator);
    ASSERT_EQUAL(updates, 0);
    ASSERT_TRUE(accumulator > 0.0f);
    return {"ApplicationFixedStep_SubStepTime_NoUpdateYet", true, ""};
}

TEST_CASE(ApplicationFixedStep_ExactStep_OneUpdate) {
    float accumulator = 0.0f;
    int updates = Engine::Application::ComputeFixedUpdateCount(1.0f / 60.0f, 1.0f / 60.0f, 0.1f, 5, accumulator);
    ASSERT_EQUAL(updates, 1);
    ASSERT_FLOAT_NEAR(accumulator, 0.0f, 0.0001f);
    return {"ApplicationFixedStep_ExactStep_OneUpdate", true, ""};
}

TEST_CASE(ApplicationFixedStep_ClampAndBudgetApplied) {
    float accumulator = 0.0f;

    // frameTime=1.0 gets clamped to 0.1. With fixedDt=0.01, that is 10 updates,
    // but budget is 5 updates per frame.
    int updates = Engine::Application::ComputeFixedUpdateCount(1.0f, 0.01f, 0.1f, 5, accumulator);

    ASSERT_EQUAL(updates, 5);
    ASSERT_FLOAT_NEAR(accumulator, 0.05f, 0.0001f);
    return {"ApplicationFixedStep_ClampAndBudgetApplied", true, ""};
}
