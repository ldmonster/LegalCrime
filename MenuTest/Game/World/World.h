#pragma once

#include "../../Engine/Core/Types.h"
#include "../../Engine/Entity/Entity.h"
#include "../Entities/Character.h"
#include <vector>
#include <memory>

namespace Engine {
    class TileMap;
    class ILogger;
}

namespace LegalCrime {
namespace World {

    /// <summary>
    /// World represents the game world, containing all entities, the tilemap, and spatial organization.
    /// Provides a container and query interface for game entities.
    /// </summary>
    class World {
    public:
        World(Engine::ILogger* logger = nullptr);
        ~World();

        // Entity management
        void AddEntity(std::unique_ptr<Engine::Entity> entity);
        void RemoveEntity(Engine::Entity* entity);
        void ClearEntities();
        
        // Entity queries
        Engine::Entity* GetEntityById(uint32_t id);
        const std::vector<Engine::Entity*>& GetAllEntities() const { return m_entityList; }
        
        // Character queries (convenience methods)
        Entities::Character* GetCharacterById(uint32_t id);
        std::vector<Entities::Character*> GetAllCharacters();
        Entities::Character* GetCharacterAtTile(uint16_t row, uint16_t col);

        // TileMap access
        void SetTileMap(Engine::TileMap* tileMap);
        Engine::TileMap* GetTileMap() { return m_tileMap; }
        const Engine::TileMap* GetTileMap() const { return m_tileMap; }

        // Update all entities
        void Update(float deltaTime);

        // Spatial queries (future: spatial hash/grid)
        std::vector<Engine::Entity*> GetEntitiesInRadius(const Engine::Point& center, float radius);

    private:
        Engine::ILogger* m_logger;

        // Entity storage
        std::vector<std::unique_ptr<Engine::Entity>> m_entities;
        std::vector<Engine::Entity*> m_entityList;  // Fast iteration (raw pointers)

        // TileMap (not owned by World - owned by scene)
        Engine::TileMap* m_tileMap;

        // Internal methods
        void RebuildEntityList();
    };

} // namespace World
} // namespace LegalCrime
