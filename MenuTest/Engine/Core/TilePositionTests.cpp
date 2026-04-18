#include "../Core/Types.h"
#include "../../Tests/SimpleTest.h"

#define PASS return SimpleTest::TestResult{__FUNCTION__, true, ""}

// ========== TilePosition Value Object Tests ==========

TEST_CASE(TilePosition_DefaultConstruction) {
    Engine::TilePosition pos;
    ASSERT_EQUAL(pos.row, static_cast<uint16_t>(0));
    ASSERT_EQUAL(pos.col, static_cast<uint16_t>(0));
    PASS;
}

TEST_CASE(TilePosition_ParameterizedConstruction) {
    Engine::TilePosition pos(5, 10);
    ASSERT_EQUAL(pos.row, static_cast<uint16_t>(5));
    ASSERT_EQUAL(pos.col, static_cast<uint16_t>(10));
    PASS;
}

TEST_CASE(TilePosition_EqualityOperator) {
    Engine::TilePosition a(3, 7);
    Engine::TilePosition b(3, 7);
    ASSERT_TRUE(a == b);
    PASS;
}

TEST_CASE(TilePosition_InequalityOperator_DifferentRow) {
    Engine::TilePosition a(3, 7);
    Engine::TilePosition b(4, 7);
    ASSERT_TRUE(a != b);
    PASS;
}

TEST_CASE(TilePosition_InequalityOperator_DifferentCol) {
    Engine::TilePosition a(3, 7);
    Engine::TilePosition b(3, 8);
    ASSERT_TRUE(a != b);
    PASS;
}

TEST_CASE(TilePosition_DistanceTo_SamePosition) {
    Engine::TilePosition a(5, 5);
    ASSERT_EQUAL(a.DistanceTo(a), 0);
    PASS;
}

TEST_CASE(TilePosition_DistanceTo_Adjacent) {
    Engine::TilePosition a(5, 5);
    Engine::TilePosition b(5, 6);
    ASSERT_EQUAL(a.DistanceTo(b), 1);
    PASS;
}

TEST_CASE(TilePosition_DistanceTo_Diagonal) {
    Engine::TilePosition a(0, 0);
    Engine::TilePosition b(3, 4);
    ASSERT_EQUAL(a.DistanceTo(b), 7);
    PASS;
}

TEST_CASE(TilePosition_IsAdjacent_True_Up) {
    Engine::TilePosition a(5, 5);
    Engine::TilePosition b(4, 5);
    ASSERT_TRUE(a.IsAdjacent(b));
    PASS;
}

TEST_CASE(TilePosition_IsAdjacent_True_Right) {
    Engine::TilePosition a(5, 5);
    Engine::TilePosition b(5, 6);
    ASSERT_TRUE(a.IsAdjacent(b));
    PASS;
}

TEST_CASE(TilePosition_IsAdjacent_False_Diagonal) {
    Engine::TilePosition a(5, 5);
    Engine::TilePosition b(6, 6);
    ASSERT_FALSE(a.IsAdjacent(b));
    PASS;
}

TEST_CASE(TilePosition_IsAdjacent_False_SamePosition) {
    Engine::TilePosition a(5, 5);
    ASSERT_FALSE(a.IsAdjacent(a));
    PASS;
}

TEST_CASE(TilePosition_Hash_EqualPositions_SameHash) {
    Engine::TilePosition a(10, 20);
    Engine::TilePosition b(10, 20);
    Engine::TilePosition::Hash hasher;
    ASSERT_EQUAL(hasher(a), hasher(b));
    PASS;
}

TEST_CASE(TilePosition_Hash_DifferentPositions_DifferentHash) {
    Engine::TilePosition a(10, 20);
    Engine::TilePosition b(20, 10);
    Engine::TilePosition::Hash hasher;
    ASSERT_TRUE(hasher(a) != hasher(b));
    PASS;
}

// ========== Direction Value Object Tests ==========

TEST_CASE(Direction_ToString_Down) {
    ASSERT_TRUE(std::string(Engine::DirectionUtil::ToString(Engine::Direction::Down)) == "down");
    PASS;
}

TEST_CASE(Direction_ToString_Up) {
    ASSERT_TRUE(std::string(Engine::DirectionUtil::ToString(Engine::Direction::Up)) == "up");
    PASS;
}

TEST_CASE(Direction_ToString_Left) {
    ASSERT_TRUE(std::string(Engine::DirectionUtil::ToString(Engine::Direction::Left)) == "left");
    PASS;
}

TEST_CASE(Direction_ToString_Right) {
    ASSERT_TRUE(std::string(Engine::DirectionUtil::ToString(Engine::Direction::Right)) == "right");
    PASS;
}

TEST_CASE(Direction_ToAnimationName_Default) {
    ASSERT_TRUE(Engine::DirectionUtil::ToAnimationName(Engine::Direction::Down) == "walk_down");
    PASS;
}

TEST_CASE(Direction_ToAnimationName_CustomPrefix) {
    ASSERT_TRUE(Engine::DirectionUtil::ToAnimationName(Engine::Direction::Up, "run") == "run_up");
    PASS;
}

TEST_CASE(Direction_Opposite) {
    ASSERT_TRUE(Engine::DirectionUtil::Opposite(Engine::Direction::Up) == Engine::Direction::Down);
    ASSERT_TRUE(Engine::DirectionUtil::Opposite(Engine::Direction::Down) == Engine::Direction::Up);
    ASSERT_TRUE(Engine::DirectionUtil::Opposite(Engine::Direction::Left) == Engine::Direction::Right);
    ASSERT_TRUE(Engine::DirectionUtil::Opposite(Engine::Direction::Right) == Engine::Direction::Left);
    PASS;
}

TEST_CASE(Direction_FromDelta_Up) {
    ASSERT_TRUE(Engine::DirectionUtil::FromDelta(-1, 0) == Engine::Direction::Up);
    PASS;
}

TEST_CASE(Direction_FromDelta_Down) {
    ASSERT_TRUE(Engine::DirectionUtil::FromDelta(1, 0) == Engine::Direction::Down);
    PASS;
}

TEST_CASE(Direction_FromDelta_Left) {
    ASSERT_TRUE(Engine::DirectionUtil::FromDelta(0, -1) == Engine::Direction::Left);
    PASS;
}

TEST_CASE(Direction_FromDelta_Right) {
    ASSERT_TRUE(Engine::DirectionUtil::FromDelta(0, 1) == Engine::Direction::Right);
    PASS;
}

TEST_CASE(Direction_FromPositions) {
    Engine::TilePosition a(5, 5);
    Engine::TilePosition b(4, 5);
    ASSERT_TRUE(Engine::DirectionUtil::FromPositions(a, b) == Engine::Direction::Up);
    PASS;
}
