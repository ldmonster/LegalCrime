#include "../../Tests/SimpleTest.h"
#include "Game/World/Systems/SelectionSystem.h"

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
