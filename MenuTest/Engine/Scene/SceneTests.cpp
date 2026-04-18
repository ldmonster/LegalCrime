#include "IScene.h"
#include "../../Tests/SimpleTest.h"

// ======================== SceneId Tests ========================

// Concrete test scene for verifying SceneId
class TestScene : public Engine::IScene {
public:
    TestScene(Engine::SceneId id) : m_id(id) {}
    Engine::Result<void> Initialize() override { return Engine::Result<void>::Success(); }
    void Shutdown() override {}
    void HandleEvent(const SDL_Event&) override {}
    void Update(float) override {}
    void Render() override {}
    const std::string& GetName() const override { static std::string n = "test"; return n; }
    bool IsInitialized() const override { return true; }
    Engine::SceneId GetSceneId() const override { return m_id; }
private:
    Engine::SceneId m_id;
};

// Scene that doesn't override GetSceneId — should return Unknown
class DefaultScene : public Engine::IScene {
public:
    Engine::Result<void> Initialize() override { return Engine::Result<void>::Success(); }
    void Shutdown() override {}
    void HandleEvent(const SDL_Event&) override {}
    void Update(float) override {}
    void Render() override {}
    const std::string& GetName() const override { static std::string n = "default"; return n; }
    bool IsInitialized() const override { return true; }
};

TEST_CASE(SceneId_DefaultIsUnknown) {
    DefaultScene scene;
    ASSERT_TRUE(scene.GetSceneId() == Engine::SceneId::Unknown);
    return {"SceneId_DefaultIsUnknown", true, ""};
}

TEST_CASE(SceneId_MainMenu) {
    TestScene scene(Engine::SceneId::MainMenu);
    ASSERT_TRUE(scene.GetSceneId() == Engine::SceneId::MainMenu);
    return {"SceneId_MainMenu", true, ""};
}

TEST_CASE(SceneId_Gameplay) {
    TestScene scene(Engine::SceneId::Gameplay);
    ASSERT_TRUE(scene.GetSceneId() == Engine::SceneId::Gameplay);
    return {"SceneId_Gameplay", true, ""};
}

TEST_CASE(SceneId_CanDistinguishScenes) {
    TestScene menu(Engine::SceneId::MainMenu);
    TestScene game(Engine::SceneId::Gameplay);

    ASSERT_TRUE(menu.GetSceneId() != game.GetSceneId());
    ASSERT_TRUE(menu.GetSceneId() == Engine::SceneId::MainMenu);
    ASSERT_TRUE(game.GetSceneId() == Engine::SceneId::Gameplay);
    return {"SceneId_CanDistinguishScenes", true, ""};
}

TEST_CASE(SceneId_StaticCast_SafeWhenMatched) {
    TestScene menu(Engine::SceneId::MainMenu);
    Engine::IScene* asInterface = &menu;

    // Verify we can safely static_cast when SceneId matches
    if (asInterface->GetSceneId() == Engine::SceneId::MainMenu) {
        auto* concrete = static_cast<TestScene*>(asInterface);
        ASSERT_NOT_NULL(concrete);
    }
    return {"SceneId_StaticCast_SafeWhenMatched", true, ""};
}
