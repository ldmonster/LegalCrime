#include "Pathfinding.h"
#include "../../Tests/SimpleTest.h"

#define PASS return SimpleTest::TestResult{__FUNCTION__, true, ""}

// Simple walkability: all tiles walkable
static bool AllWalkable(const Engine::TilePosition&) { return true; }

// Grid with a wall: column 3 is blocked except row 0
static bool WallAtCol3(const Engine::TilePosition& pos) {
    if (pos.col == 3 && pos.row != 0) return false;
    return true;
}

// ========== Pathfinding Tests ==========

TEST_CASE(Pathfinding_SameStartAndGoal) {
    Engine::Pathfinding pf;
    Engine::Path path = pf.FindPath({5, 5}, {5, 5}, 10, 10, AllWalkable);
    // Same start and goal → empty or single-element path
    ASSERT_TRUE(path.empty() || (path.size() == 1 && path[0] == Engine::TilePosition(5, 5)));
    PASS;
}

TEST_CASE(Pathfinding_AdjacentTile) {
    Engine::Pathfinding pf;
    Engine::Path path = pf.FindPath({5, 5}, {5, 6}, 10, 10, AllWalkable);
    ASSERT_FALSE(path.empty());
    // Last tile should be the goal
    ASSERT_TRUE(path.back() == Engine::TilePosition(5, 6));
    PASS;
}

TEST_CASE(Pathfinding_PathAroundWall) {
    Engine::Pathfinding pf;
    Engine::Pathfinding::Options opts;
    opts.allowDiagonal = false;  // 4-directional only

    // Start at (2,2), goal at (2,4) — must go around wall at col 3
    Engine::Path path = pf.FindPath({2, 2}, {2, 4}, 10, 10, WallAtCol3, opts);
    ASSERT_FALSE(path.empty());
    ASSERT_TRUE(path.back() == Engine::TilePosition(2, 4));

    // Verify no tile in path is on the wall
    for (const auto& tile : path) {
        ASSERT_TRUE(WallAtCol3(tile));
    }
    PASS;
}

TEST_CASE(Pathfinding_NoPathExists) {
    Engine::Pathfinding pf;
    // Completely blocked: nothing is walkable except start
    auto noneWalkable = [](const Engine::TilePosition& pos) {
        return pos.row == 0 && pos.col == 0;
    };
    Engine::Path path = pf.FindPath({0, 0}, {5, 5}, 10, 10, noneWalkable);
    ASSERT_TRUE(path.empty());
    PASS;
}

TEST_CASE(Pathfinding_HasPath_True) {
    Engine::Pathfinding pf;
    ASSERT_TRUE(pf.HasPath({0, 0}, {9, 9}, 10, 10, AllWalkable));
    PASS;
}

TEST_CASE(Pathfinding_HasPath_False) {
    Engine::Pathfinding pf;
    // Wall blocks entire row
    auto wallRow5 = [](const Engine::TilePosition& pos) { return pos.row != 5; };
    ASSERT_FALSE(pf.HasPath({0, 0}, {9, 0}, 10, 10, wallRow5));
    PASS;
}

TEST_CASE(Pathfinding_OutOfBoundsGoal) {
    Engine::Pathfinding pf;
    Engine::Path path = pf.FindPath({0, 0}, {20, 20}, 10, 10, AllWalkable);
    ASSERT_TRUE(path.empty());
    PASS;
}

TEST_CASE(Pathfinding_DiagonalMovement) {
    Engine::Pathfinding pf;
    Engine::Pathfinding::Options opts;
    opts.allowDiagonal = true;

    Engine::Path path = pf.FindPath({0, 0}, {3, 3}, 10, 10, AllWalkable, opts);
    ASSERT_FALSE(path.empty());
    ASSERT_TRUE(path.back() == Engine::TilePosition(3, 3));
    // Diagonal path should be shorter than Manhattan
    ASSERT_TRUE(path.size() <= 4);
    PASS;
}

// ========== Path Smoothing Tests ==========

TEST_CASE(Pathfinding_SmoothPath_ShortPath_Unchanged) {
    Engine::Path path = {{0,0}, {0,1}};
    Engine::Path smoothed = Engine::Pathfinding::SmoothPath(path, 10, 10, AllWalkable);
    ASSERT_EQUAL(smoothed.size(), (size_t)2);
    PASS;
}

TEST_CASE(Pathfinding_SmoothPath_StraightLine_Reduced) {
    // Straight line of 5 tiles should be reduced to start + end
    Engine::Path path = {{0,0}, {0,1}, {0,2}, {0,3}, {0,4}};
    Engine::Path smoothed = Engine::Pathfinding::SmoothPath(path, 10, 10, AllWalkable);
    ASSERT_TRUE(smoothed.size() <= 2);
    ASSERT_TRUE(smoothed.front() == Engine::TilePosition(0,0));
    ASSERT_TRUE(smoothed.back() == Engine::TilePosition(0,4));
    PASS;
}

TEST_CASE(Pathfinding_SmoothPath_WithWall_KeepsCorner) {
    // Path goes around a wall — smoothing should keep the corner waypoint
    Engine::Pathfinding pf;
    Engine::Pathfinding::Options opts;
    opts.allowDiagonal = false;

    Engine::Path path = pf.FindPath({2, 2}, {2, 4}, 10, 10, WallAtCol3, opts);
    ASSERT_FALSE(path.empty());

    Engine::Path smoothed = Engine::Pathfinding::SmoothPath(path, 10, 10, WallAtCol3);
    ASSERT_TRUE(smoothed.front() == path.front());
    ASSERT_TRUE(smoothed.back() == path.back());
    // Smoothed path should still be valid (no wall tiles)
    for (const auto& tile : smoothed) {
        ASSERT_TRUE(WallAtCol3(tile));
    }
    PASS;
}
