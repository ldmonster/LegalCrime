#include "../../../Tests/SimpleTest.h"
#include "../../World/Systems/CommandSystem.h"
#include "../../World/Systems/MovementSystem.h"
#include "../../World/Commands/Command.h"
#include "../../World/World.h"
#include "../../Entities/Character.h"
#include "../../../Engine/Graphics/CharacterSpriteConfig.h"

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
    MoveCommand cmd(Engine::TilePosition(5, 5));
    sys.IssueCommand(1, cmd);
    ASSERT_TRUE(sys.HasCommands(1));
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CommandSystem_IssueCommand_ReplacesExisting) {
    CommandSystem sys;
    MoveCommand cmd1(Engine::TilePosition(1, 1));
    MoveCommand cmd2(Engine::TilePosition(9, 9));
    sys.IssueCommand(1, cmd1);
    sys.IssueCommand(1, cmd2);
    const CommandQueue* q = sys.GetQueue(1);
    ASSERT_NOT_NULL(q);
    ASSERT_EQUAL(q->GetCount(), (size_t)1);
    const GameCommand* front = q->GetCurrentCommand();
    ASSERT_NOT_NULL(front);
    const MoveCommand* move = std::get_if<MoveCommand>(front);
    ASSERT_NOT_NULL(move);
    ASSERT_EQUAL(move->target.row, (uint16_t)9);
    ASSERT_EQUAL(move->target.col, (uint16_t)9);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CommandSystem_QueueCommand_Appends) {
    CommandSystem sys;
    MoveCommand cmd1(Engine::TilePosition(1, 1));
    MoveCommand cmd2(Engine::TilePosition(2, 2));
    MoveCommand cmd3(Engine::TilePosition(3, 3));
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
    MoveCommand cmd1(Engine::TilePosition(1, 1));
    MoveCommand cmd2(Engine::TilePosition(2, 2));
    sys.IssueCommand(1, cmd1);
    sys.QueueCommand(1, cmd2);
    const CommandQueue* q = sys.GetQueue(1);
    ASSERT_NOT_NULL(q);
    const GameCommand* front = q->GetCurrentCommand();
    ASSERT_NOT_NULL(front);
    const MoveCommand* move = std::get_if<MoveCommand>(front);
    ASSERT_NOT_NULL(move);
    ASSERT_EQUAL(move->target.row, (uint16_t)1);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CommandSystem_CancelCommands_ClearsQueue) {
    CommandSystem sys;
    MoveCommand cmd(Engine::TilePosition(5, 5));
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
    MoveCommand cmd1(Engine::TilePosition(1, 1));
    AttackCommand cmd2(42);
    sys.IssueCommand(1, cmd1);
    sys.IssueCommand(2, cmd2);
    ASSERT_TRUE(sys.HasCommands(1));
    ASSERT_TRUE(sys.HasCommands(2));
    sys.CancelCommands(1);
    ASSERT_FALSE(sys.HasCommands(1));
    ASSERT_TRUE(sys.HasCommands(2));
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CommandSystem_Update_DeadEntityCommandBecomesNoOp) {
    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    Engine::CharacterSpriteConfig config;
    auto character = std::make_unique<LegalCrime::Entities::Character>(
        LegalCrime::Entities::CharacterType::Thug,
        nullptr,
        config,
        nullptr
    );

    auto* spawned = world.SpawnCharacter(std::move(character), Engine::TilePosition(2, 2));
    ASSERT_NOT_NULL(spawned);

    CommandSystem sys;
    MovementSystem movement;
    MoveCommand moveCmd(Engine::TilePosition(5, 5));
    sys.IssueCommand(spawned->GetId(), moveCmd);
    ASSERT_TRUE(sys.HasCommands(spawned->GetId()));

    ASSERT_TRUE(world.DestroyEntityById(spawned->GetId()));

    // CommandSystem should treat dead-target commands as completed no-ops.
    sys.Update(&world, &movement, 0.016f);
    ASSERT_FALSE(sys.HasCommands(spawned->GetId()));

    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}
