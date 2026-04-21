#include "SceneManager.h"
#include "../../Tests/SimpleTest.h"

namespace {

class LifecycleTestScene : public Engine::IScene {
public:
    explicit LifecycleTestScene(const std::string& name) : m_name(name) {}

    Engine::Result<void> Initialize() override {
        ++initializeCount;
        initialized = true;
        return Engine::Result<void>::Success();
    }

    void Shutdown() override {
        ++shutdownCount;
        initialized = false;
    }

    void HandleEvent(const SDL_Event&) override {}
    void Update(float) override {}
    void Render() override {}

    const std::string& GetName() const override { return m_name; }
    bool IsInitialized() const override { return initialized; }

    void OnEnter() override { ++enterCount; }
    void OnExit() override { ++exitCount; }
    void OnPause() override { ++pauseCount; }
    void OnResume() override { ++resumeCount; }

    int initializeCount = 0;
    int shutdownCount = 0;
    int enterCount = 0;
    int exitCount = 0;
    int pauseCount = 0;
    int resumeCount = 0;

private:
    std::string m_name;
    bool initialized = false;
};

} // namespace

TEST_CASE(SceneManager_Push_CallsInitializeAndEnter) {
    Engine::SceneManager manager(nullptr);
    auto scene = std::make_unique<LifecycleTestScene>("A");
    auto* raw = scene.get();

    manager.PushScene(std::move(scene));

    ASSERT_EQUAL(raw->initializeCount, 1);
    ASSERT_EQUAL(raw->enterCount, 1);
    ASSERT_TRUE(manager.GetCurrentScene() == raw);
    return {"SceneManager_Push_CallsInitializeAndEnter", true, ""};
}

TEST_CASE(SceneManager_PushSecond_PausesFirst) {
    Engine::SceneManager manager(nullptr);

    auto first = std::make_unique<LifecycleTestScene>("A");
    auto* firstRaw = first.get();
    manager.PushScene(std::move(first));

    auto second = std::make_unique<LifecycleTestScene>("B");
    auto* secondRaw = second.get();
    manager.PushScene(std::move(second));

    ASSERT_EQUAL(firstRaw->pauseCount, 1);
    ASSERT_EQUAL(secondRaw->enterCount, 1);
    ASSERT_TRUE(manager.GetCurrentScene() == secondRaw);
    return {"SceneManager_PushSecond_PausesFirst", true, ""};
}

TEST_CASE(SceneManager_Pop_ExitsCurrentAndResumesPrevious) {
    Engine::SceneManager manager(nullptr);

    auto first = std::make_unique<LifecycleTestScene>("A");
    auto* firstRaw = first.get();
    manager.PushScene(std::move(first));

    auto second = std::make_unique<LifecycleTestScene>("B");
    auto* secondRaw = second.get();
    manager.PushScene(std::move(second));

    manager.PopScene();

    ASSERT_EQUAL(secondRaw->exitCount, 1);
    ASSERT_EQUAL(secondRaw->shutdownCount, 1);
    ASSERT_EQUAL(firstRaw->resumeCount, 1);
    ASSERT_TRUE(manager.GetCurrentScene() == firstRaw);
    return {"SceneManager_Pop_ExitsCurrentAndResumesPrevious", true, ""};
}

TEST_CASE(SceneManager_Replace_ExitsOld_EntersNew) {
    Engine::SceneManager manager(nullptr);

    auto first = std::make_unique<LifecycleTestScene>("A");
    auto* firstRaw = first.get();
    manager.PushScene(std::move(first));

    auto replacement = std::make_unique<LifecycleTestScene>("B");
    auto* replacementRaw = replacement.get();
    manager.ReplaceScene(std::move(replacement));

    ASSERT_EQUAL(firstRaw->exitCount, 1);
    ASSERT_EQUAL(firstRaw->shutdownCount, 1);
    ASSERT_EQUAL(replacementRaw->initializeCount, 1);
    ASSERT_EQUAL(replacementRaw->enterCount, 1);
    ASSERT_TRUE(manager.GetCurrentScene() == replacementRaw);
    return {"SceneManager_Replace_ExitsOld_EntersNew", true, ""};
}
