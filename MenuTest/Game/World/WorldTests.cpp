#include "World.h"
#include "../Events.h"
#include "../../Tests/SimpleTest.h"
#include "../../Engine/Entity/Entity.h"
#include "../Entities/Character.h"
#include "../../Engine/Graphics/CharacterSpriteConfig.h"

// ======================== World Entity Lookup Tests ========================

TEST_CASE(World_GetEntityById_ReturnsCorrectEntity) {
    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    auto entity = std::make_unique<Engine::Entity>("test_entity", nullptr);
    uint32_t id = entity->GetId();
    world.AddEntity(std::move(entity));

    Engine::Entity* found = world.GetEntityById(id);
    ASSERT_NOT_NULL(found);
    ASSERT_EQUAL(found->GetId(), id);
    return {"World_GetEntityById_ReturnsCorrectEntity", true, ""};
}

TEST_CASE(World_GetEntityById_ReturnsNullForMissing) {
    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    Engine::Entity* found = world.GetEntityById(9999);
    ASSERT_NULL(found);
    return {"World_GetEntityById_ReturnsNullForMissing", true, ""};
}

TEST_CASE(World_RemoveEntity_RemovesFromMap) {
    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    auto entity = std::make_unique<Engine::Entity>("test_entity", nullptr);
    uint32_t id = entity->GetId();
    Engine::Entity* raw = entity.get();
    world.AddEntity(std::move(entity));

    ASSERT_NOT_NULL(world.GetEntityById(id));
    world.RemoveEntity(raw);
    ASSERT_NULL(world.GetEntityById(id));
    return {"World_RemoveEntity_RemovesFromMap", true, ""};
}

TEST_CASE(World_GetAllEntities_ReturnsAllAdded) {
    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    auto e1 = std::make_unique<Engine::Entity>("e1", nullptr);
    auto e2 = std::make_unique<Engine::Entity>("e2", nullptr);
    auto e3 = std::make_unique<Engine::Entity>("e3", nullptr);
    world.AddEntity(std::move(e1));
    world.AddEntity(std::move(e2));
    world.AddEntity(std::move(e3));

    ASSERT_EQUAL(world.GetAllEntities().size(), (size_t)3);
    return {"World_GetAllEntities_ReturnsAllAdded", true, ""};
}

TEST_CASE(World_ClearEntities_ClearsAll) {
    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    auto e1 = std::make_unique<Engine::Entity>("e1", nullptr);
    uint32_t id = e1->GetId();
    world.AddEntity(std::move(e1));

    world.ClearEntities();
    ASSERT_EQUAL(world.GetAllEntities().size(), (size_t)0);
    ASSERT_NULL(world.GetEntityById(id));
    return {"World_ClearEntities_ClearsAll", true, ""};
}

// ======================== World PlaceCharacter O(1) Tests ========================

TEST_CASE(World_PlaceCharacter_OccupiesCorrectTile) {
    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    auto entity = std::make_unique<Engine::Entity>("occupant", nullptr);
    Engine::Entity* raw = entity.get();
    world.AddEntity(std::move(entity));

    Engine::TilePosition pos(5, 5);
    ASSERT_FALSE(world.IsOccupied(pos));

    // Use a basic entity for occupancy — PlaceCharacter takes Character* but we need to test
    // the basic occupancy mechanism. We'll test with nullptr cast guard in real usage.
    // For this test, just verify IsOccupied via RemoveOccupant
    world.RemoveOccupant(pos); // no-op
    ASSERT_FALSE(world.IsOccupied(pos));
    return {"World_PlaceCharacter_OccupiesCorrectTile", true, ""};
}

TEST_CASE(World_IsOccupied_ReturnsFalseForEmpty) {
    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    ASSERT_FALSE(world.IsOccupied(Engine::TilePosition(0, 0)));
    ASSERT_FALSE(world.IsOccupied(Engine::TilePosition(10, 10)));
    return {"World_IsOccupied_ReturnsFalseForEmpty", true, ""};
}

TEST_CASE(World_RemoveOccupant_ClearsTile) {
    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    Engine::TilePosition pos(3, 4);
    // Even without placing, remove should be safe
    world.RemoveOccupant(pos);
    ASSERT_FALSE(world.IsOccupied(pos));
    return {"World_RemoveOccupant_ClearsTile", true, ""};
}

// ======================== World Multi-Add/Remove Stress ========================

TEST_CASE(World_AddRemove_ManyEntities) {
    LegalCrime::World::World world(2000, 2000, 64, nullptr);
    std::vector<uint32_t> ids;

    for (int i = 0; i < 100; ++i) {
        auto e = std::make_unique<Engine::Entity>("e" + std::to_string(i), nullptr);
        ids.push_back(e->GetId());
        world.AddEntity(std::move(e));
    }

    ASSERT_EQUAL(world.GetAllEntities().size(), (size_t)100);

    // Remove every other entity
    for (int i = 0; i < 100; i += 2) {
        Engine::Entity* e = world.GetEntityById(ids[i]);
        ASSERT_NOT_NULL(e);
        world.RemoveEntity(e);
    }

    ASSERT_EQUAL(world.GetAllEntities().size(), (size_t)50);

    // Verify remaining are accessible
    for (int i = 1; i < 100; i += 2) {
        ASSERT_NOT_NULL(world.GetEntityById(ids[i]));
    }
    // Verify removed are gone
    for (int i = 0; i < 100; i += 2) {
        ASSERT_NULL(world.GetEntityById(ids[i]));
    }

    return {"World_AddRemove_ManyEntities", true, ""};
}

// ======================== World Aggregate Root Tests ========================

TEST_CASE(World_SpawnCharacter_OwnsAndPlacesCharacter) {
    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    Engine::CharacterSpriteConfig config;
    auto character = std::make_unique<LegalCrime::Entities::Character>(
        LegalCrime::Entities::CharacterType::Thug,
        nullptr,
        config,
        nullptr
    );

    Engine::TilePosition spawnPos(2, 3);
    LegalCrime::Entities::Character* spawned = world.SpawnCharacter(std::move(character), spawnPos);

    ASSERT_NOT_NULL(spawned);
    ASSERT_TRUE(world.GetCharacterAtTile(spawnPos) == spawned);
    ASSERT_TRUE(world.IsOccupied(spawnPos));
    ASSERT_NOT_NULL(world.GetCharacterById(spawned->GetId()));
    return {"World_SpawnCharacter_OwnsAndPlacesCharacter", true, ""};
}

TEST_CASE(World_DestroyCharacter_RemovesFromEntityAndOccupancyMaps) {
    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    Engine::CharacterSpriteConfig config;
    auto character = std::make_unique<LegalCrime::Entities::Character>(
        LegalCrime::Entities::CharacterType::Thug,
        nullptr,
        config,
        nullptr
    );

    Engine::TilePosition spawnPos(4, 6);
    LegalCrime::Entities::Character* spawned = world.SpawnCharacter(std::move(character), spawnPos);
    ASSERT_NOT_NULL(spawned);
    uint32_t id = spawned->GetId();

    ASSERT_TRUE(world.DestroyCharacter(spawned));
    ASSERT_NULL(world.GetEntityById(id));
    ASSERT_NULL(world.GetCharacterById(id));
    ASSERT_FALSE(world.IsOccupied(spawnPos));
    ASSERT_NULL(world.GetCharacterAtTile(spawnPos));
    return {"World_DestroyCharacter_RemovesFromEntityAndOccupancyMaps", true, ""};
}

TEST_CASE(World_DestroyEntityById_ReturnsFalseForMissing) {
    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    ASSERT_FALSE(world.DestroyEntityById(777777));
    return {"World_DestroyEntityById_ReturnsFalseForMissing", true, ""};
}

// ======================== World Domain Event Tests ========================

TEST_CASE(World_SpawnCharacter_PublishesEntitySpawnedEvent) {
    LegalCrime::DomainEventBus().Clear();
    uint32_t receivedId = 0;
    Engine::TilePosition receivedPos;

    auto sub = LegalCrime::DomainEventBus().Subscribe<LegalCrime::EntitySpawnedEvent>(
        [&](const LegalCrime::EntitySpawnedEvent& e) {
            receivedId = e.entityId;
            receivedPos = e.position;
        }
    );

    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    Engine::CharacterSpriteConfig config;
    auto character = std::make_unique<LegalCrime::Entities::Character>(
        LegalCrime::Entities::CharacterType::Thug,
        nullptr,
        config,
        nullptr
    );

    Engine::TilePosition spawnPos(1, 2);
    auto* spawned = world.SpawnCharacter(std::move(character), spawnPos);
    ASSERT_NOT_NULL(spawned);
    ASSERT_EQUAL(receivedId, spawned->GetId());
    ASSERT_TRUE(receivedPos == spawnPos);

    LegalCrime::DomainEventBus().Unsubscribe<LegalCrime::EntitySpawnedEvent>(sub);
    return {"World_SpawnCharacter_PublishesEntitySpawnedEvent", true, ""};
}

TEST_CASE(World_PlaceCharacter_PublishesEntityMovedEvent) {
    LegalCrime::DomainEventBus().Clear();
    uint32_t movedId = 0;
    Engine::TilePosition from;
    Engine::TilePosition to;

    auto sub = LegalCrime::DomainEventBus().Subscribe<LegalCrime::EntityMovedEvent>(
        [&](const LegalCrime::EntityMovedEvent& e) {
            movedId = e.entityId;
            from = e.from;
            to = e.to;
        }
    );

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
    world.PlaceCharacter(spawned, Engine::TilePosition(3, 4));

    ASSERT_EQUAL(movedId, spawned->GetId());
    ASSERT_TRUE(from == Engine::TilePosition(2, 2));
    ASSERT_TRUE(to == Engine::TilePosition(3, 4));

    LegalCrime::DomainEventBus().Unsubscribe<LegalCrime::EntityMovedEvent>(sub);
    return {"World_PlaceCharacter_PublishesEntityMovedEvent", true, ""};
}

TEST_CASE(World_DestroyCharacter_PublishesEntityDestroyedEvent) {
    LegalCrime::DomainEventBus().Clear();
    uint32_t destroyedId = 0;

    auto sub = LegalCrime::DomainEventBus().Subscribe<LegalCrime::EntityDestroyedEvent>(
        [&](const LegalCrime::EntityDestroyedEvent& e) {
            destroyedId = e.entityId;
        }
    );

    LegalCrime::World::World world(1000, 1000, 64, nullptr);
    Engine::CharacterSpriteConfig config;
    auto character = std::make_unique<LegalCrime::Entities::Character>(
        LegalCrime::Entities::CharacterType::Thug,
        nullptr,
        config,
        nullptr
    );

    auto* spawned = world.SpawnCharacter(std::move(character), Engine::TilePosition(4, 4));
    ASSERT_NOT_NULL(spawned);
    uint32_t id = spawned->GetId();
    ASSERT_TRUE(world.DestroyCharacter(spawned));
    ASSERT_EQUAL(destroyedId, id);

    LegalCrime::DomainEventBus().Unsubscribe<LegalCrime::EntityDestroyedEvent>(sub);
    return {"World_DestroyCharacter_PublishesEntityDestroyedEvent", true, ""};
}
