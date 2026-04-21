#include "../../../Tests/SimpleTest.h"
#include "SelectionSystem.h"
#include "../World.h"
#include "../../Entities/Character.h"
#include "../../../../Engine/World/TileMap.h"
#include "../../../../Engine/Graphics/CharacterSpriteConfig.h"
#include <chrono>

TEST_CASE(SelectionBench_BoxSelect_500Characters) {
    LegalCrime::World::World world(4000, 4000, 64, nullptr);
    Engine::TileMap tileMap(40, 40, nullptr);
    auto initResult = tileMap.Initialize(800, 600);
    ASSERT_TRUE(initResult.success);

    Engine::CharacterSpriteConfig config;
    for (int i = 0; i < 500; ++i) {
        auto ch = std::make_unique<LegalCrime::Entities::Character>(
            LegalCrime::Entities::CharacterType::Thug,
            nullptr,
            config,
            nullptr
        );
        uint16_t row = static_cast<uint16_t>(i % 40);
        uint16_t col = static_cast<uint16_t>((i * 3) % 40);
        world.SpawnCharacter(std::move(ch), Engine::TilePosition(row, col));
    }

    LegalCrime::World::SelectionSystem selection(nullptr);

    auto start = std::chrono::high_resolution_clock::now();
    selection.StartBoxSelection(Engine::Point(0, 0));
    selection.UpdateBoxSelection(Engine::Point(800, 600));
    selection.EndBoxSelection(&world, &tileMap, nullptr, false);
    auto end = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    ASSERT_TRUE(selection.GetSelectionCount() > 0);
    ASSERT_TRUE(ms < 1000);
    return {"SelectionBench_BoxSelect_500Characters", true, ""};
}
