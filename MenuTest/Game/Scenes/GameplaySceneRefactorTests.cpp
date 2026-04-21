#include "GameplayScene.h"
#include "GameplayInputHandler.h"
#include "GameplayRenderer.h"
#include "../../Tests/SimpleTest.h"

TEST_CASE(GameplayScene_Refactor_SceneId) {
    LegalCrime::GameplayScene scene(nullptr, nullptr, nullptr, nullptr);
    ASSERT_TRUE(scene.GetSceneId() == Engine::SceneId::Gameplay);
    return {"GameplayScene_Refactor_SceneId", true, ""};
}

TEST_CASE(GameplayScene_Refactor_NotInitializedByDefault) {
    LegalCrime::GameplayScene scene(nullptr, nullptr, nullptr, nullptr);
    ASSERT_FALSE(scene.IsInitialized());
    return {"GameplayScene_Refactor_NotInitializedByDefault", true, ""};
}

TEST_CASE(GameplayScene_Refactor_PublicCallsSafeBeforeInit) {
    LegalCrime::GameplayScene scene(nullptr, nullptr, nullptr, nullptr);
    scene.MoveCharacterToTile(Engine::TilePosition(1, 1), 0.2f);
    scene.MoveCharacterToTile(1, 1, 0.2f);
    scene.SelectCharacterAt(100, 100);
    scene.CommandMoveToPosition(100, 100);
    scene.Update(0.016f);
    scene.Render();
    ASSERT_FALSE(scene.IsInitialized());
    return {"GameplayScene_Refactor_PublicCallsSafeBeforeInit", true, ""};
}

TEST_CASE(GameplayInputHandler_NullSafeMethods) {
    LegalCrime::GameplayInputHandler handler(nullptr, nullptr);
    handler.HandleInputActions(nullptr, nullptr, nullptr);
    handler.SelectCharacterAt(nullptr, 0, 0, nullptr, nullptr);
    handler.CommandMoveToPosition(nullptr, 0, 0, nullptr, nullptr);
    ASSERT_TRUE(true);
    return {"GameplayInputHandler_NullSafeMethods", true, ""};
}

TEST_CASE(GameplayRenderer_NullSafeMethods) {
    LegalCrime::GameplayRenderer renderer(nullptr, nullptr);
    renderer.SyncEntityScreenPositions(nullptr, nullptr, nullptr);
    renderer.Render(nullptr, nullptr, nullptr, nullptr);
    ASSERT_TRUE(true);
    return {"GameplayRenderer_NullSafeMethods", true, ""};
}
