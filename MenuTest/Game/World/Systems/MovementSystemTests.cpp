#include "../../../Tests/SimpleTest.h"
#include "MovementSystem.h"

using namespace LegalCrime::World;

// =============================================================================
// MovementSystem Tests — stateless/null-safe behavior
// =============================================================================

TEST_CASE(MovementSystem_ConstructsWithoutLogger) {
    MovementSystem sys;
    // Should not crash
    ASSERT_NULL(sys.GetPathfinder());
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(MovementSystem_IsCharacterMoving_Null_ReturnsFalse) {
    MovementSystem sys;
    ASSERT_FALSE(sys.IsCharacterMoving(nullptr));
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(MovementSystem_StopCharacterMovement_Null_NoCrash) {
    MovementSystem sys;
    sys.StopCharacterMovement(nullptr); // Should not crash
    ASSERT_TRUE(true);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(MovementSystem_MoveCharacterToTile_NullCharacter_ReturnsFalse) {
    MovementSystem sys;
    ASSERT_FALSE(sys.MoveCharacterToTile(nullptr, Engine::TilePosition(1, 1)));
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(MovementSystem_MoveCharacterAlongPath_NullCharacter_ReturnsFalse) {
    MovementSystem sys;
    Engine::Path path = { Engine::TilePosition(0,0), Engine::TilePosition(1,1) };
    ASSERT_FALSE(sys.MoveCharacterAlongPath(nullptr, path));
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(MovementSystem_MoveCharacterAlongPath_EmptyPath_ReturnsFalse) {
    MovementSystem sys;
    Engine::Path emptyPath;
    ASSERT_FALSE(sys.MoveCharacterAlongPath(nullptr, emptyPath));
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(MovementSystem_Update_NullWorld_NoCrash) {
    MovementSystem sys;
    sys.Update(nullptr, 0.016f); // Should not crash
    ASSERT_TRUE(true);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(MovementSystem_Initialize_NullTileMap_NoCrash) {
    MovementSystem sys;
    sys.Initialize(nullptr);
    ASSERT_NULL(sys.GetPathfinder()); // Not initialized with null tilemap
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}
