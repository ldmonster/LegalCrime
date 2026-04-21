#include "../../Tests/SimpleTest.h"
#include "Game/World/Systems/SelectionSystem.h"
#include "Game/World/World.h"
#include "Engine/Graphics/CharacterSpriteConfig.h"

// We test the selection data management (not input handling which requires SDL).

// Minimal mock character for selection tests
#include "Engine/Entity/Entity.h"
#include "Game/Entities/Character.h"

// Use SelectionSystem's non-input methods

TEST_CASE(SelectionSystem_StartsEmpty) {
    LegalCrime::World::SelectionSystem sel;
    ASSERT_FALSE(sel.HasSelection());
    ASSERT_EQUAL(sel.GetSelectionCount(), (size_t)0);
    ASSERT_NULL(sel.GetSelectedCharacter());
    return {"SelectionSystem_StartsEmpty", true, ""};
}

TEST_CASE(SelectionSystem_ClearSelection) {
    LegalCrime::World::SelectionSystem sel;
    sel.ClearSelection();
    ASSERT_FALSE(sel.HasSelection());
    return {"SelectionSystem_ClearSelection", true, ""};
}

TEST_CASE(SelectionSystem_BoxSelectRect) {
    LegalCrime::World::SelectionSystem sel;
    // Box select rect when not selecting returns default
    Engine::Rect rect = sel.GetBoxSelectRect();
    ASSERT_EQUAL(rect.w, 0);
    ASSERT_EQUAL(rect.h, 0);
    return {"SelectionSystem_BoxSelectRect", true, ""};
}

TEST_CASE(SelectionSystem_IsBoxSelecting_DefaultFalse) {
    LegalCrime::World::SelectionSystem sel;
    ASSERT_FALSE(sel.IsBoxSelecting());
    return {"SelectionSystem_IsBoxSelecting_DefaultFalse", true, ""};
}

TEST_CASE(SelectionSystem_SaveRecallGroup_EmptyGroup) {
    LegalCrime::World::SelectionSystem sel;
    sel.SaveGroup(0);
    sel.RecallGroup(0);
    ASSERT_FALSE(sel.HasSelection());
    return {"SelectionSystem_SaveRecallGroup_EmptyGroup", true, ""};
}

TEST_CASE(SelectionSystem_InvalidGroupIndex) {
    LegalCrime::World::SelectionSystem sel;
    // Should not crash on out-of-bounds
    sel.SaveGroup(-1);
    sel.SaveGroup(10);
    sel.RecallGroup(-1);
    sel.RecallGroup(10);
    ASSERT_FALSE(sel.HasSelection());
    return {"SelectionSystem_InvalidGroupIndex", true, ""};
}

TEST_CASE(SelectionSystem_HoveredTile_DefaultFalse) {
    LegalCrime::World::SelectionSystem sel;
    uint16_t row, col;
    ASSERT_FALSE(sel.GetHoveredTile(row, col));
    auto opt = sel.GetHoveredTilePosition();
    ASSERT_FALSE(opt.has_value());
    return {"SelectionSystem_HoveredTile_DefaultFalse", true, ""};
}

TEST_CASE(SelectionSystem_BoxSelectionLifecycle) {
    LegalCrime::World::SelectionSystem sel;
    sel.StartBoxSelection(Engine::Point(10, 10));
    ASSERT_TRUE(sel.IsBoxSelecting());

    sel.UpdateBoxSelection(Engine::Point(30, 40));
    Engine::Rect rect = sel.GetBoxSelectRect();
    ASSERT_EQUAL(rect.x, 10);
    ASSERT_EQUAL(rect.y, 10);
    ASSERT_EQUAL(rect.w, 20);
    ASSERT_EQUAL(rect.h, 30);

    // Null-safe end call should stop box selection.
    sel.EndBoxSelection(nullptr, nullptr, nullptr, false);
    ASSERT_FALSE(sel.IsBoxSelecting());
    return {"SelectionSystem_BoxSelectionLifecycle", true, ""};
}

TEST_CASE(SelectionSystem_SelectAtScreenPosition_NullSafe) {
    LegalCrime::World::SelectionSystem sel;
    sel.SelectAtScreenPosition(nullptr, 0, 0, nullptr, nullptr, false, true);
    ASSERT_FALSE(sel.HasSelection());
    return {"SelectionSystem_SelectAtScreenPosition_NullSafe", true, ""};
}

TEST_CASE(SelectionSystem_UpdateHoveredTile_NullMapClearsHover) {
    LegalCrime::World::SelectionSystem sel;
    sel.UpdateHoveredTile(100, 200, nullptr, nullptr);
    uint16_t row = 0, col = 0;
    ASSERT_FALSE(sel.GetHoveredTile(row, col));
    return {"SelectionSystem_UpdateHoveredTile_NullMapClearsHover", true, ""};
}

TEST_CASE(SelectionSystem_PruneSelectionToWorld_RemovesDeadCharacters) {
    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    Engine::CharacterSpriteConfig config;
    auto character = std::make_unique<LegalCrime::Entities::Character>(
        LegalCrime::Entities::CharacterType::Thug,
        nullptr,
        config,
        nullptr
    );

    auto* spawned = world.SpawnCharacter(std::move(character), Engine::TilePosition(3, 3));
    ASSERT_NOT_NULL(spawned);

    LegalCrime::World::SelectionSystem sel;
    sel.SelectCharacter(spawned);
    ASSERT_TRUE(sel.HasSelection());

    ASSERT_TRUE(world.DestroyCharacter(spawned));
    sel.PruneSelectionToWorld(&world);
    ASSERT_FALSE(sel.HasSelection());

    return {"SelectionSystem_PruneSelectionToWorld_RemovesDeadCharacters", true, ""};
}

TEST_CASE(SelectionSystem_PruneSelectionToWorld_NullWorldClearsSelection) {
    LegalCrime::World::SelectionSystem sel;
    Engine::CharacterSpriteConfig config;
    auto character = std::make_unique<LegalCrime::Entities::Character>(
        LegalCrime::Entities::CharacterType::Thug,
        nullptr,
        config,
        nullptr
    );

    auto* raw = character.get();
    sel.SelectCharacter(raw);
    ASSERT_TRUE(sel.HasSelection());

    sel.PruneSelectionToWorld(nullptr);
    ASSERT_FALSE(sel.HasSelection());
    return {"SelectionSystem_PruneSelectionToWorld_NullWorldClearsSelection", true, ""};
}
