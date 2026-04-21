#include "GameSimulation.h"
#include "../../Tests/SimpleTest.h"

TEST_CASE(GameSimulation_DefaultState) {
    LegalCrime::Simulation::GameSimulation simulation(nullptr, nullptr);
    ASSERT_FALSE(simulation.IsInitialized());
    ASSERT_NULL(simulation.GetWorld());
    ASSERT_NULL(simulation.GetMovementSystem());
    ASSERT_NULL(simulation.GetSelectionSystem());
    ASSERT_NULL(simulation.GetCommandSystem());
    ASSERT_NULL(simulation.GetPrimaryCharacter());
    return {"GameSimulation_DefaultState", true, ""};
}

TEST_CASE(GameSimulation_MoveBeforeInitialize_Fails) {
    LegalCrime::Simulation::GameSimulation simulation(nullptr, nullptr);
    bool moved = simulation.MoveCharacterToTile(Engine::TilePosition(0, 0), 0.3f);
    ASSERT_FALSE(moved);
    return {"GameSimulation_MoveBeforeInitialize_Fails", true, ""};
}

TEST_CASE(GameSimulation_InitializeWithNullTileMap_Fails) {
    LegalCrime::Simulation::GameSimulation simulation(nullptr, nullptr);
    auto result = simulation.Initialize(nullptr);
    ASSERT_FALSE(result.success);
    ASSERT_FALSE(simulation.IsInitialized());
    return {"GameSimulation_InitializeWithNullTileMap_Fails", true, ""};
}
