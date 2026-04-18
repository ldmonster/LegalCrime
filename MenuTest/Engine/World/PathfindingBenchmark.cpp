#include "../../Tests/SimpleTest.h"
#include "../World/Pathfinding.h"
#include <chrono>

// =============================================================================
// PathfindingBenchmark — performance tests for many concurrent pathfind calls
// =============================================================================

namespace {
    // All tiles walkable
    bool AllWalkable(const Engine::TilePosition&) { return true; }

    // Walkable except a horizontal wall in the middle
    bool WallInMiddle50(const Engine::TilePosition& pos) {
        // Wall at row 25, except a gap at col 48-49
        if (pos.row == 25 && pos.col < 48) return false;
        return true;
    }
}

TEST_CASE(PathfindingBench_100_Paths_OpenMap) {
    Engine::Pathfinding pf;
    const uint16_t mapSize = 50;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 100; i++) {
        uint16_t startCol = static_cast<uint16_t>(i % mapSize);
        Engine::TilePosition from(0, startCol);
        Engine::TilePosition to(mapSize - 1, (mapSize - 1) - startCol);
        auto path = pf.FindPath(from, to, mapSize, mapSize, AllWalkable);
        ASSERT_FALSE(path.empty());
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Should complete 100 paths on a 50x50 open map well under 5 seconds
    ASSERT_TRUE(ms < 5000);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(PathfindingBench_100_Paths_WithWall) {
    Engine::Pathfinding pf;
    const uint16_t mapSize = 50;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 100; i++) {
        uint16_t startCol = static_cast<uint16_t>(i % mapSize);
        Engine::TilePosition from(0, startCol);
        Engine::TilePosition to(mapSize - 1, (mapSize - 1) - startCol);
        auto path = pf.FindPath(from, to, mapSize, mapSize, WallInMiddle50);
        ASSERT_FALSE(path.empty());
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    ASSERT_TRUE(ms < 5000);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(PathfindingBench_LargeMap_SinglePath) {
    Engine::Pathfinding pf;
    const uint16_t mapSize = 200;

    auto start = std::chrono::high_resolution_clock::now();

    Engine::TilePosition from(0, 0);
    Engine::TilePosition to(mapSize - 1, mapSize - 1);
    auto path = pf.FindPath(from, to, mapSize, mapSize, AllWalkable);
    ASSERT_FALSE(path.empty());

    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Single path on 200x200 should complete well under 5 seconds
    ASSERT_TRUE(ms < 5000);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}
