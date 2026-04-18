#pragma once

#include "../../Engine/Core/Types.h"
#include "../../Engine/Entity/Entity.h"
#include "../../Engine/World/SpatialGrid.h"
#include "../Entities/Character.h"
#include <vector>
#include <memory>
#include <unordered_map>

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
        World(uint16_t worldWidth, uint16_t worldHeight, uint16_t cellSize = 64, Engine::ILogger* logger = nullptr);
        ~World();

        // Entity management
        void AddEntity(std::unique_ptr<Engine::Entity> entity);
        void RemoveEntity(Engine::Entity* entity);
        void ClearEntities();
        
        // Entity queries
        Engine::Entity* GetEntityById(uint32_t id);
        const std::vector<Engine::Entity*>& GetAllEntities() const { return m_entityList; }
        
        // Character queries (O(1) lookups via dedicated maps)
        Entities::Character* GetCharacterById(uint32_t id);
        const std::vector<Entities::Character*>& GetAllCharacters() const { return m_characters; }
        Entities::Character* GetCharacterAtTile(const Engine::TilePosition& pos);
        Entities::Character* GetCharacterAtTile(uint16_t row, uint16_t col);

        // Tile occupancy management
        bool IsOccupied(const Engine::TilePosition& pos) const;
        void PlaceCharacter(Entities::Character* character, const Engine::TilePosition& pos);
        void RemoveOccupant(const Engine::TilePosition& pos);

        // TileMap access
        void SetTileMap(Engine::TileMap* tileMap);
        Engine::TileMap* GetTileMap() { return m_tileMap; }
        const Engine::TileMap* GetTileMap() const { return m_tileMap; }

        // Update all entities
        void Update(float deltaTime);

        // Spatial queries (uses SpatialGrid for O(k) lookups)
        std::vector<Engine::Entity*> GetEntitiesInRadius(const Engine::Point& center, float radius);
        std::vector<Engine::Entity*> GetEntitiesInRect(const Engine::Rect& rect);

        // Access the spatial grid directly (for advanced usage)
        Engine::SpatialGrid& GetSpatialGrid() { return m_spatialGrid; }

    private:
        Engine::ILogger* m_logger;

        // Entity storage — O(1) lookup via hash map, O(n) iteration via dense list
        std::vector<std::unique_ptr<Engine::Entity>> m_entities;
        std::vector<Engine::Entity*> m_entityList;  // Fast iteration (raw pointers)
        std::unordered_map<uint32_t, Engine::Entity*> m_entityMap;  // O(1) by ID

        // Character-specific list — avoids dynamic_cast scans
        std::vector<Entities::Character*> m_characters;

        // Reverse lookup: entity ID → tile position (O(1) PlaceCharacter old-pos removal)
        std::unordered_map<uint32_t, Engine::TilePosition> m_entityPositions;

        // TileMap (not owned by World - owned by scene)
        Engine::TileMap* m_tileMap;

        // Spatial grid for fast proximity queries
        Engine::SpatialGrid m_spatialGrid;

        // Tile occupancy map for O(1) lookups
        std::unordered_map<Engine::TilePosition, Entities::Character*, Engine::TilePosition::Hash> m_occupancy;
    };

} // namespace World
} // namespace LegalCrime
