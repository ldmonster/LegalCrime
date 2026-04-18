#include "../../../Tests/SimpleTest.h"
#include "../../World/Systems/CommandSystem.h"
#include "../../World/Commands/Command.h"
#include "../../World/Commands/MoveCommand.h"
#include "../../World/Commands/AttackCommand.h"

using namespace LegalCrime::World;

// =============================================================================
// CommandSystem Tests — queue management, issue, cancel, shift-queue
// =============================================================================

TEST_CASE(CommandSystem_InitiallyNoCommands) {
    CommandSystem sys;
    ASSERT_FALSE(sys.HasCommands(1));
    ASSERT_NULL(sys.GetQueue(1));
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CommandSystem_IssueCommand_HasCommands) {
    CommandSystem sys;
    Command cmd(CommandType::Move, Engine::TilePosition(5, 5));
    sys.IssueCommand(1, cmd);
    ASSERT_TRUE(sys.HasCommands(1));
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CommandSystem_IssueCommand_ReplacesExisting) {
    CommandSystem sys;
    Command cmd1(CommandType::Move, Engine::TilePosition(1, 1));
    Command cmd2(CommandType::Move, Engine::TilePosition(9, 9));
    sys.IssueCommand(1, cmd1);
    sys.IssueCommand(1, cmd2);
    const CommandQueue* q = sys.GetQueue(1);
    ASSERT_NOT_NULL(q);
    ASSERT_EQUAL(q->GetCount(), (size_t)1);
    const Command* front = q->GetCurrentCommand();
    ASSERT_NOT_NULL(front);
    ASSERT_EQUAL(front->targetPosition.row, (uint16_t)9);
    ASSERT_EQUAL(front->targetPosition.col, (uint16_t)9);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CommandSystem_QueueCommand_Appends) {
    CommandSystem sys;
    Command cmd1(CommandType::Move, Engine::TilePosition(1, 1));
    Command cmd2(CommandType::Move, Engine::TilePosition(2, 2));
    Command cmd3(CommandType::Move, Engine::TilePosition(3, 3));
    sys.IssueCommand(1, cmd1);
    sys.QueueCommand(1, cmd2);
    sys.QueueCommand(1, cmd3);
    const CommandQueue* q = sys.GetQueue(1);
    ASSERT_NOT_NULL(q);
    ASSERT_EQUAL(q->GetCount(), (size_t)3);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CommandSystem_QueueCommand_FIFO_Order) {
    CommandSystem sys;
    Command cmd1(CommandType::Move, Engine::TilePosition(1, 1));
    Command cmd2(CommandType::Move, Engine::TilePosition(2, 2));
    sys.IssueCommand(1, cmd1);
    sys.QueueCommand(1, cmd2);
    const CommandQueue* q = sys.GetQueue(1);
    ASSERT_NOT_NULL(q);
    const Command* front = q->GetCurrentCommand();
    ASSERT_NOT_NULL(front);
    ASSERT_EQUAL(front->targetPosition.row, (uint16_t)1);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CommandSystem_CancelCommands_ClearsQueue) {
    CommandSystem sys;
    Command cmd(CommandType::Move, Engine::TilePosition(5, 5));
    sys.IssueCommand(1, cmd);
    sys.QueueCommand(1, cmd);
    ASSERT_TRUE(sys.HasCommands(1));
    sys.CancelCommands(1);
    ASSERT_FALSE(sys.HasCommands(1));
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CommandSystem_CancelCommands_NoEffect_OnUnknownEntity) {
    CommandSystem sys;
    sys.CancelCommands(999); // Should not crash
    ASSERT_FALSE(sys.HasCommands(999));
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CommandSystem_MultipleEntities_Independent) {
    CommandSystem sys;
    Command cmd1(CommandType::Move, Engine::TilePosition(1, 1));
    Command cmd2(CommandType::Attack, Engine::TilePosition(9, 9), 42);
    sys.IssueCommand(1, cmd1);
    sys.IssueCommand(2, cmd2);
    ASSERT_TRUE(sys.HasCommands(1));
    ASSERT_TRUE(sys.HasCommands(2));
    sys.CancelCommands(1);
    ASSERT_FALSE(sys.HasCommands(1));
    ASSERT_TRUE(sys.HasCommands(2));
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}
