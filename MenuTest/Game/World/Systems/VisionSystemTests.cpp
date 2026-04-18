#include "../../Tests/SimpleTest.h"
#include "Game/World/Systems/VisionSystem.h"

TEST_CASE(VisionSystem_StartsUnexplored) {
    LegalCrime::World::VisionSystem vis(10, 10);
    ASSERT_TRUE(vis.GetVisibility(0, 0) == LegalCrime::World::TileVisibility::Unexplored);
    ASSERT_TRUE(vis.GetVisibility(5, 5) == LegalCrime::World::TileVisibility::Unexplored);
    ASSERT_FALSE(vis.IsVisible(0, 0));
    ASSERT_FALSE(vis.IsExplored(0, 0));
    return {"VisionSystem_StartsUnexplored", true, ""};
}

TEST_CASE(VisionSystem_RevealAround) {
    LegalCrime::World::VisionSystem vis(20, 20);
    vis.RevealAround(Engine::TilePosition(10, 10), 3);

    ASSERT_TRUE(vis.IsVisible(10, 10));  // center
    ASSERT_TRUE(vis.IsVisible(10, 12));  // within radius
    ASSERT_TRUE(vis.IsVisible(8, 10));   // within radius
    ASSERT_FALSE(vis.IsVisible(0, 0));   // far away
    return {"VisionSystem_RevealAround", true, ""};
}

TEST_CASE(VisionSystem_BeginFrame_DemotesToExplored) {
    LegalCrime::World::VisionSystem vis(10, 10);
    vis.RevealAround(Engine::TilePosition(5, 5), 2);
    ASSERT_TRUE(vis.IsVisible(5, 5));

    vis.BeginFrame();
    ASSERT_FALSE(vis.IsVisible(5, 5));
    ASSERT_TRUE(vis.IsExplored(5, 5));
    ASSERT_TRUE(vis.GetVisibility(5, 5) == LegalCrime::World::TileVisibility::Explored);
    return {"VisionSystem_BeginFrame_DemotesToExplored", true, ""};
}

TEST_CASE(VisionSystem_ExploredStaysExplored) {
    LegalCrime::World::VisionSystem vis(10, 10);
    vis.RevealAround(Engine::TilePosition(5, 5), 1);
    vis.BeginFrame(); // Visible → Explored
    vis.BeginFrame(); // Explored stays Explored
    ASSERT_TRUE(vis.IsExplored(5, 5));
    ASSERT_TRUE(vis.GetVisibility(5, 5) == LegalCrime::World::TileVisibility::Explored);
    return {"VisionSystem_ExploredStaysExplored", true, ""};
}

TEST_CASE(VisionSystem_RevealAll) {
    LegalCrime::World::VisionSystem vis(5, 5);
    vis.RevealAll();
    for (uint16_t r = 0; r < 5; ++r) {
        for (uint16_t c = 0; c < 5; ++c) {
            ASSERT_TRUE(vis.IsVisible(r, c));
        }
    }
    return {"VisionSystem_RevealAll", true, ""};
}

TEST_CASE(VisionSystem_Reset) {
    LegalCrime::World::VisionSystem vis(5, 5);
    vis.RevealAll();
    vis.Reset();
    for (uint16_t r = 0; r < 5; ++r) {
        for (uint16_t c = 0; c < 5; ++c) {
            ASSERT_TRUE(vis.GetVisibility(r, c) == LegalCrime::World::TileVisibility::Unexplored);
        }
    }
    return {"VisionSystem_Reset", true, ""};
}

TEST_CASE(VisionSystem_OutOfBounds_ReturnsUnexplored) {
    LegalCrime::World::VisionSystem vis(10, 10);
    vis.RevealAll();
    ASSERT_TRUE(vis.GetVisibility(100, 100) == LegalCrime::World::TileVisibility::Unexplored);
    return {"VisionSystem_OutOfBounds_ReturnsUnexplored", true, ""};
}

TEST_CASE(VisionSystem_CircularReveal) {
    LegalCrime::World::VisionSystem vis(20, 20);
    vis.RevealAround(Engine::TilePosition(10, 10), 2);

    // Corner at distance sqrt(8) ≈ 2.83 should NOT be visible (radius=2)
    ASSERT_FALSE(vis.IsVisible(8, 8));
    // Adjacent at distance 2 SHOULD be visible
    ASSERT_TRUE(vis.IsVisible(10, 12));
    ASSERT_TRUE(vis.IsVisible(12, 10));
    return {"VisionSystem_CircularReveal", true, ""};
}
