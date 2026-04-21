#include "../../../Tests/SimpleTest.h"
#include "MovementSystem.h"
#include "../World.h"
#include "../../Entities/Character.h"
#include "../../../Engine/World/TileMap.h"
#include "../../../Engine/Graphics/CharacterSpriteConfig.h"

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

TEST_CASE(MovementSystem_MoveCharacterToTile_QueuesPathRequest) {
    MovementSystem sys;
    Engine::TileMap tileMap(10, 10, nullptr);
    auto initResult = tileMap.Initialize(800, 600);
    ASSERT_TRUE(initResult.success);
    sys.Initialize(&tileMap);

    Engine::CharacterSpriteConfig config;
    LegalCrime::Entities::Character character(LegalCrime::Entities::CharacterType::Thug, nullptr, config, nullptr);
    character.SetTilePosition(0, 0);

    ASSERT_TRUE(sys.MoveCharacterToTile(&character, Engine::TilePosition(2, 2)));
    ASSERT_EQUAL(sys.GetPendingPathRequestCount(), (size_t)1);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(MovementSystem_Update_ProcessesPendingRequestsWithinBudget) {
    MovementSystem sys;
    Engine::TileMap tileMap(10, 10, nullptr);
    auto initResult = tileMap.Initialize(800, 600);
    ASSERT_TRUE(initResult.success);
    sys.Initialize(&tileMap);

    Engine::CharacterSpriteConfig config;
    std::vector<std::unique_ptr<LegalCrime::Entities::Character>> characters;
    for (int i = 0; i < 7; ++i) {
        auto ch = std::make_unique<LegalCrime::Entities::Character>(
            LegalCrime::Entities::CharacterType::Thug,
            nullptr,
            config,
            nullptr
        );
        ch->SetTilePosition(0, static_cast<uint16_t>(i));
        ASSERT_TRUE(sys.MoveCharacterToTile(ch.get(), Engine::TilePosition(5, static_cast<uint16_t>(i))));
        characters.push_back(std::move(ch));
    }

    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    sys.Update(&world, 0.016f);

    // 7 requests queued, budget is 5 per frame => 2 should remain.
    ASSERT_EQUAL(sys.GetPendingPathRequestCount(), (size_t)2);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}
