#include "../../Tests/SimpleTest.h"
#include "EntityManager.h"
#include "ComponentStorage.h"
#include "System.h"

// ======================== EntityManager Tests ========================

TEST_CASE(EntityManager_CreateEntity_ReturnsNonZeroId) {
    Engine::ECS::EntityManager mgr;
    auto handle = mgr.CreateEntity();
    ASSERT_TRUE(handle.id != Engine::ECS::INVALID_ENTITY);
    return {"EntityManager_CreateEntity_ReturnsNonZeroId", true, ""};
}

TEST_CASE(EntityManager_CreateEntity_UniqueIds) {
    Engine::ECS::EntityManager mgr;
    auto a = mgr.CreateEntity();
    auto b = mgr.CreateEntity();
    auto c = mgr.CreateEntity();
    ASSERT_TRUE(a != b);
    ASSERT_TRUE(b != c);
    ASSERT_TRUE(a != c);
    return {"EntityManager_CreateEntity_UniqueIds", true, ""};
}

TEST_CASE(EntityManager_IsAlive) {
    Engine::ECS::EntityManager mgr;
    auto handle = mgr.CreateEntity();
    ASSERT_TRUE(mgr.IsAlive(handle));
    ASSERT_FALSE(mgr.IsAlive(Engine::ECS::INVALID_HANDLE));
    ASSERT_FALSE(mgr.IsAlive(Engine::ECS::EntityHandle(999, 0)));
    return {"EntityManager_IsAlive", true, ""};
}

TEST_CASE(EntityManager_DestroyEntity) {
    Engine::ECS::EntityManager mgr;
    auto handle = mgr.CreateEntity();
    ASSERT_TRUE(mgr.IsAlive(handle));
    mgr.DestroyEntity(handle);
    ASSERT_FALSE(mgr.IsAlive(handle));
    return {"EntityManager_DestroyEntity", true, ""};
}

TEST_CASE(EntityManager_GetEntityCount) {
    Engine::ECS::EntityManager mgr;
    ASSERT_EQUAL(mgr.GetEntityCount(), (size_t)0);
    auto a = mgr.CreateEntity();
    auto b = mgr.CreateEntity();
    ASSERT_EQUAL(mgr.GetEntityCount(), (size_t)2);
    mgr.DestroyEntity(a);
    ASSERT_EQUAL(mgr.GetEntityCount(), (size_t)1);
    mgr.DestroyEntity(b);
    ASSERT_EQUAL(mgr.GetEntityCount(), (size_t)0);
    return {"EntityManager_GetEntityCount", true, ""};
}

TEST_CASE(EntityManager_RecyclesIds) {
    Engine::ECS::EntityManager mgr;
    auto a = mgr.CreateEntity();
    mgr.DestroyEntity(a);
    auto b = mgr.CreateEntity();
    // Recycled ID should equal the destroyed one's id, but version incremented
    ASSERT_EQUAL(b.id, a.id);
    ASSERT_TRUE(b.version > a.version);
    ASSERT_TRUE(mgr.IsAlive(b));
    // Old handle should be stale
    ASSERT_FALSE(mgr.IsAlive(a));
    return {"EntityManager_RecyclesIds", true, ""};
}

TEST_CASE(EntityManager_ForEach) {
    Engine::ECS::EntityManager mgr;
    auto a = mgr.CreateEntity();
    auto b = mgr.CreateEntity();
    auto c = mgr.CreateEntity();
    mgr.DestroyEntity(b);

    std::vector<Engine::ECS::EntityId> visited;
    mgr.ForEach([&](Engine::ECS::EntityId id) { visited.push_back(id); });
    ASSERT_EQUAL(visited.size(), (size_t)2);
    // Should contain a and c (b was destroyed)
    bool foundA = false, foundC = false;
    for (auto id : visited) {
        if (id == a.id) foundA = true;
        if (id == c.id) foundC = true;
    }
    ASSERT_TRUE(foundA);
    ASSERT_TRUE(foundC);
    return {"EntityManager_ForEach", true, ""};
}

// ======================== ComponentStorage Tests ========================

struct TestComponent {
    int value = 0;
    float speed = 0.0f;
};

TEST_CASE(ComponentStorage_Add_And_Get) {
    Engine::ECS::ComponentStorage<TestComponent> storage;
    Engine::ECS::EntityId id = 1;
    storage.Add(id, TestComponent{42, 1.5f});
    auto* comp = storage.Get(id);
    ASSERT_NOT_NULL(comp);
    ASSERT_EQUAL(comp->value, 42);
    return {"ComponentStorage_Add_And_Get", true, ""};
}

TEST_CASE(ComponentStorage_Get_Missing_ReturnsNull) {
    Engine::ECS::ComponentStorage<TestComponent> storage;
    ASSERT_NULL(storage.Get(999));
    return {"ComponentStorage_Get_Missing_ReturnsNull", true, ""};
}

TEST_CASE(ComponentStorage_Has) {
    Engine::ECS::ComponentStorage<TestComponent> storage;
    Engine::ECS::EntityId id = 1;
    ASSERT_FALSE(storage.Has(id));
    storage.Add(id, TestComponent{10, 0.0f});
    ASSERT_TRUE(storage.Has(id));
    return {"ComponentStorage_Has", true, ""};
}

TEST_CASE(ComponentStorage_Remove) {
    Engine::ECS::ComponentStorage<TestComponent> storage;
    storage.Add(1, TestComponent{10, 0.0f});
    storage.Add(2, TestComponent{20, 0.0f});
    storage.Add(3, TestComponent{30, 0.0f});
    ASSERT_EQUAL(storage.Size(), (size_t)3);

    storage.Remove(2);
    ASSERT_EQUAL(storage.Size(), (size_t)2);
    ASSERT_NULL(storage.Get(2));

    // Other entities still accessible
    ASSERT_NOT_NULL(storage.Get(1));
    ASSERT_NOT_NULL(storage.Get(3));
    ASSERT_EQUAL(storage.Get(1)->value, 10);
    ASSERT_EQUAL(storage.Get(3)->value, 30);
    return {"ComponentStorage_Remove", true, ""};
}

TEST_CASE(ComponentStorage_Add_Overwrites) {
    Engine::ECS::ComponentStorage<TestComponent> storage;
    storage.Add(1, TestComponent{10, 0.0f});
    storage.Add(1, TestComponent{99, 0.0f});
    ASSERT_EQUAL(storage.Size(), (size_t)1);
    ASSERT_EQUAL(storage.Get(1)->value, 99);
    return {"ComponentStorage_Add_Overwrites", true, ""};
}

TEST_CASE(ComponentStorage_ForEach) {
    Engine::ECS::ComponentStorage<TestComponent> storage;
    storage.Add(1, TestComponent{10, 0.0f});
    storage.Add(2, TestComponent{20, 0.0f});
    storage.Add(3, TestComponent{30, 0.0f});

    int sum = 0;
    storage.ForEach([&](Engine::ECS::EntityId, TestComponent& comp) {
        sum += comp.value;
    });
    ASSERT_EQUAL(sum, 60);
    return {"ComponentStorage_ForEach", true, ""};
}

TEST_CASE(ComponentStorage_Clear) {
    Engine::ECS::ComponentStorage<TestComponent> storage;
    storage.Add(1, TestComponent{10, 0.0f});
    storage.Add(2, TestComponent{20, 0.0f});
    storage.Clear();
    ASSERT_EQUAL(storage.Size(), (size_t)0);
    ASSERT_NULL(storage.Get(1));
    return {"ComponentStorage_Clear", true, ""};
}

TEST_CASE(ComponentStorage_DenseArrayIteration) {
    Engine::ECS::ComponentStorage<TestComponent> storage;
    storage.Add(5, TestComponent{50, 0.0f});
    storage.Add(10, TestComponent{100, 0.0f});

    const auto& dense = storage.GetDenseArray();
    ASSERT_EQUAL(dense.size(), (size_t)2);
    return {"ComponentStorage_DenseArrayIteration", true, ""};
}
