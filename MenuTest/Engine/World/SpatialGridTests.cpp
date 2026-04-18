#include "../../Tests/SimpleTest.h"
#include "SpatialGrid.h"
#include "../Entity/Entity.h"

using namespace Engine;
using namespace SimpleTest;

TEST_CASE(SpatialGrid_InsertAndQuery) {
    SpatialGrid grid(1000, 1000, 100);

    Entity e("test");
    e.SetPosition(150, 150);
    grid.Insert(&e);

    auto found = grid.QueryRadius(Point(150, 150), 10.0f);
    ASSERT_EQUAL(static_cast<int>(found.size()), 1);
    ASSERT_TRUE(found[0] == &e);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SpatialGrid_QueryRadius_ExcludesFarEntities) {
    SpatialGrid grid(1000, 1000, 100);

    Entity near("near");
    near.SetPosition(50, 50);
    grid.Insert(&near);

    Entity far("far");
    far.SetPosition(900, 900);
    grid.Insert(&far);

    auto found = grid.QueryRadius(Point(50, 50), 100.0f);
    ASSERT_EQUAL(static_cast<int>(found.size()), 1);
    ASSERT_TRUE(found[0] == &near);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SpatialGrid_Remove) {
    SpatialGrid grid(500, 500, 50);

    Entity e("test");
    e.SetPosition(100, 100);
    grid.Insert(&e);
    grid.Remove(&e);

    auto found = grid.QueryRadius(Point(100, 100), 50.0f);
    ASSERT_EQUAL(static_cast<int>(found.size()), 0);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SpatialGrid_Update_MovesCell) {
    SpatialGrid grid(1000, 1000, 100);

    Entity e("mover");
    e.SetPosition(50, 50);
    grid.Insert(&e);

    // Move entity to a different cell
    Point oldPos = e.GetPosition();
    e.SetPosition(550, 550);
    grid.Update(&e, oldPos);

    // Should not be found at old position
    auto atOld = grid.QueryRadius(Point(50, 50), 60.0f);
    ASSERT_EQUAL(static_cast<int>(atOld.size()), 0);

    // Should be found at new position
    auto atNew = grid.QueryRadius(Point(550, 550), 60.0f);
    ASSERT_EQUAL(static_cast<int>(atNew.size()), 1);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SpatialGrid_Clear) {
    SpatialGrid grid(500, 500, 50);

    Entity a("a"); a.SetPosition(10, 10); grid.Insert(&a);
    Entity b("b"); b.SetPosition(100, 100); grid.Insert(&b);

    grid.Clear();

    auto found = grid.QueryRadius(Point(50, 50), 500.0f);
    ASSERT_EQUAL(static_cast<int>(found.size()), 0);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SpatialGrid_QueryRect) {
    SpatialGrid grid(1000, 1000, 100);

    Entity inside("in"); inside.SetPosition(50, 50); grid.Insert(&inside);
    Entity outside("out"); outside.SetPosition(500, 500); grid.Insert(&outside);

    auto found = grid.QueryRect(Rect(0, 0, 100, 100));
    ASSERT_EQUAL(static_cast<int>(found.size()), 1);
    ASSERT_TRUE(found[0] == &inside);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SpatialGrid_GetEntitiesAt) {
    SpatialGrid grid(1000, 1000, 100);

    Entity e("test"); e.SetPosition(150, 250); grid.Insert(&e);

    const auto& cell = grid.GetEntitiesAt(150, 250);
    ASSERT_EQUAL(static_cast<int>(cell.size()), 1);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(SpatialGrid_MultipleInSameCell) {
    SpatialGrid grid(1000, 1000, 100);

    Entity a("a"); a.SetPosition(10, 10); grid.Insert(&a);
    Entity b("b"); b.SetPosition(20, 20); grid.Insert(&b);
    Entity c("c"); c.SetPosition(30, 30); grid.Insert(&c);

    auto found = grid.QueryRadius(Point(15, 15), 50.0f);
    ASSERT_EQUAL(static_cast<int>(found.size()), 3);
    return TestResult{__FUNCTION__, true, ""};
}
