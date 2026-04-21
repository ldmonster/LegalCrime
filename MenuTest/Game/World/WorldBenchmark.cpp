#include "../../Tests/SimpleTest.h"
#include "World.h"
#include "../../Engine/Entity/Entity.h"
#include <chrono>
#include <vector>

TEST_CASE(WorldBench_1000_EntityLookup_HotLoop) {
    LegalCrime::World::World world(2000, 2000, 64, nullptr);

    std::vector<uint32_t> ids;
    ids.reserve(1000);
    for (int i = 0; i < 1000; ++i) {
        auto e = std::make_unique<Engine::Entity>("bench", nullptr);
        ids.push_back(e->GetId());
        world.AddEntity(std::move(e));
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (int r = 0; r < 100; ++r) {
        for (uint32_t id : ids) {
            auto* found = world.GetEntityById(id);
            ASSERT_NOT_NULL(found);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    // 100k O(1) lookups should be comfortably below 2 seconds in CI-like env.
    ASSERT_TRUE(us < 2000000);
    return {"WorldBench_1000_EntityLookup_HotLoop", true, ""};
}

TEST_CASE(WorldBench_SpawnDestroy_Churn) {
    LegalCrime::World::World world(2000, 2000, 64, nullptr);

    auto start = std::chrono::high_resolution_clock::now();
    for (int frame = 0; frame < 60; ++frame) {
        std::vector<Engine::Entity*> spawned;
        spawned.reserve(100);

        for (int i = 0; i < 100; ++i) {
            auto e = std::make_unique<Engine::Entity>("churn", nullptr);
            spawned.push_back(e.get());
            world.AddEntity(std::move(e));
        }

        for (auto* e : spawned) {
            world.RemoveEntity(e);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    ASSERT_TRUE(ms < 5000);
    return {"WorldBench_SpawnDestroy_Churn", true, ""};
}
