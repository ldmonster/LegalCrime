#include "ResourceManager.h"
#include "../../Tests/SimpleTest.h"

TEST_CASE(ResourceManager_DefaultState_NullManagers) {
    Engine::Resources::ResourceManager manager(nullptr, nullptr, nullptr);
    ASSERT_NULL(manager.GetTextureManager());
    ASSERT_NULL(manager.GetAudioManager());
    ASSERT_EQUAL(manager.GetTotalTextureMemory(), (size_t)0);
    return {"ResourceManager_DefaultState_NullManagers", true, ""};
}

TEST_CASE(ResourceManager_Initialize_CreatesManagers) {
    Engine::Resources::ResourceManager manager(nullptr, nullptr, nullptr);
    manager.Initialize();
    ASSERT_NOT_NULL(manager.GetTextureManager());
    ASSERT_NOT_NULL(manager.GetAudioManager());
    return {"ResourceManager_Initialize_CreatesManagers", true, ""};
}

TEST_CASE(ResourceManager_Shutdown_ResetsManagers) {
    Engine::Resources::ResourceManager manager(nullptr, nullptr, nullptr);
    manager.Initialize();
    manager.Shutdown();
    ASSERT_NULL(manager.GetTextureManager());
    ASSERT_NULL(manager.GetAudioManager());
    return {"ResourceManager_Shutdown_ResetsManagers", true, ""};
}

TEST_CASE(ResourceManager_ConvenienceMethods_BeforeInitialize_AreSafe) {
    Engine::Resources::ResourceManager manager(nullptr, nullptr, nullptr);

    ASSERT_NULL(manager.LoadTexture("x", "missing.png"));
    ASSERT_NULL(manager.GetTexture("x"));
    ASSERT_NULL(manager.LoadSound("s", "missing.wav"));
    ASSERT_NULL(manager.GetSound("s"));

    auto regResult = manager.RegisterMusic("m", "missing.ogg");
    ASSERT_FALSE(regResult.success);

    manager.PlaySound("s");
    manager.PlayMusic("m");
    manager.PreloadTextures({{"a", "b"}});
    manager.UnloadTextures();
    manager.UnloadAudio();
    manager.UnloadAll();

    ASSERT_TRUE(true);
    return {"ResourceManager_ConvenienceMethods_BeforeInitialize_AreSafe", true, ""};
}
