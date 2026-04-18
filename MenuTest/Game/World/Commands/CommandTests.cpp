#include "../../Tests/SimpleTest.h"
#include "Game/World/Commands/Command.h"
#include "Game/World/Commands/MoveCommand.h"
#include "Game/World/Commands/AttackCommand.h"

// ======================== CommandQueue Tests ========================

TEST_CASE(CommandQueue_StartsEmpty) {
    LegalCrime::World::CommandQueue queue;
    ASSERT_TRUE(queue.IsEmpty());
    ASSERT_EQUAL(queue.GetCount(), (size_t)0);
    ASSERT_NULL(queue.GetCurrentCommand());
    return {"CommandQueue_StartsEmpty", true, ""};
}

TEST_CASE(CommandQueue_SetCommand_ReplacesAll) {
    LegalCrime::World::CommandQueue queue;
    LegalCrime::World::Command cmd1(LegalCrime::World::CommandType::Move, Engine::TilePosition(1, 1));
    LegalCrime::World::Command cmd2(LegalCrime::World::CommandType::Move, Engine::TilePosition(2, 2));
    queue.QueueCommand(cmd1);
    queue.QueueCommand(cmd2);
    ASSERT_EQUAL(queue.GetCount(), (size_t)2);

    LegalCrime::World::Command cmd3(LegalCrime::World::CommandType::Stop, Engine::TilePosition());
    queue.SetCommand(cmd3);
    ASSERT_EQUAL(queue.GetCount(), (size_t)1);
    ASSERT_TRUE(queue.GetCurrentCommand()->type == LegalCrime::World::CommandType::Stop);
    return {"CommandQueue_SetCommand_ReplacesAll", true, ""};
}

TEST_CASE(CommandQueue_QueueCommand_FIFO) {
    LegalCrime::World::CommandQueue queue;
    LegalCrime::World::Command cmd1(LegalCrime::World::CommandType::Move, Engine::TilePosition(1, 1));
    LegalCrime::World::Command cmd2(LegalCrime::World::CommandType::Move, Engine::TilePosition(5, 5));
    queue.QueueCommand(cmd1);
    queue.QueueCommand(cmd2);

    ASSERT_EQUAL(queue.GetCount(), (size_t)2);
    auto* front = queue.GetCurrentCommand();
    ASSERT_NOT_NULL(front);
    ASSERT_EQUAL(front->targetPosition.row, (uint16_t)1);

    queue.PopCommand();
    front = queue.GetCurrentCommand();
    ASSERT_NOT_NULL(front);
    ASSERT_EQUAL(front->targetPosition.row, (uint16_t)5);
    return {"CommandQueue_QueueCommand_FIFO", true, ""};
}

TEST_CASE(CommandQueue_PopCommand_Empty) {
    LegalCrime::World::CommandQueue queue;
    queue.PopCommand(); // should not crash
    ASSERT_TRUE(queue.IsEmpty());
    return {"CommandQueue_PopCommand_Empty", true, ""};
}

TEST_CASE(CommandQueue_Clear) {
    LegalCrime::World::CommandQueue queue;
    queue.QueueCommand(LegalCrime::World::Command(LegalCrime::World::CommandType::Move, Engine::TilePosition(1,1)));
    queue.QueueCommand(LegalCrime::World::Command(LegalCrime::World::CommandType::Attack, Engine::TilePosition(2,2)));
    queue.Clear();
    ASSERT_TRUE(queue.IsEmpty());
    ASSERT_EQUAL(queue.GetCount(), (size_t)0);
    return {"CommandQueue_Clear", true, ""};
}

// ======================== MoveCommand Tests ========================

TEST_CASE(MoveCommand_DefaultValues) {
    LegalCrime::World::MoveCommand cmd;
    ASSERT_TRUE(cmd.type == LegalCrime::World::CommandType::Move);
    ASSERT_FLOAT_NEAR(cmd.moveDuration, 0.3f, 0.001f);
    return {"MoveCommand_DefaultValues", true, ""};
}

TEST_CASE(MoveCommand_WithTarget) {
    LegalCrime::World::MoveCommand cmd(Engine::TilePosition(5, 10), 0.5f);
    ASSERT_TRUE(cmd.type == LegalCrime::World::CommandType::Move);
    ASSERT_EQUAL(cmd.targetPosition.row, (uint16_t)5);
    ASSERT_EQUAL(cmd.targetPosition.col, (uint16_t)10);
    ASSERT_FLOAT_NEAR(cmd.moveDuration, 0.5f, 0.001f);
    return {"MoveCommand_WithTarget", true, ""};
}

// ======================== AttackCommand Tests ========================

TEST_CASE(AttackCommand_DefaultValues) {
    LegalCrime::World::AttackCommand cmd;
    ASSERT_TRUE(cmd.type == LegalCrime::World::CommandType::Attack);
    ASSERT_FLOAT_NEAR(cmd.attackRange, 1.0f, 0.001f);
    return {"AttackCommand_DefaultValues", true, ""};
}

TEST_CASE(AttackCommand_WithTarget) {
    LegalCrime::World::AttackCommand cmd(42, 3.0f);
    ASSERT_TRUE(cmd.type == LegalCrime::World::CommandType::Attack);
    ASSERT_EQUAL(cmd.targetEntityId, (uint32_t)42);
    ASSERT_FLOAT_NEAR(cmd.attackRange, 3.0f, 0.001f);
    return {"AttackCommand_WithTarget", true, ""};
}
