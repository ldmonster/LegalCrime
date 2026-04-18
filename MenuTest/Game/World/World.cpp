#include "World.h"
#include "../../Engine/World/TileMap.h"
#include "../../Engine/Core/Logger/ILogger.h"
#include "../Entities/Character.h"
#include <algorithm>
#include <cmath>

namespace LegalCrime {
namespace World {

    World::World(uint16_t worldWidth, uint16_t worldHeight, uint16_t cellSize, Engine::ILogger* logger)
        : m_logger(logger)
        , m_tileMap(nullptr)
        , m_spatialGrid(worldWidth, worldHeight, cellSize) {
        
        if (m_logger) {
            m_logger->Debug("World created");
        }
    }

    World::~World() {
        ClearEntities();
        
        if (m_logger) {
            m_logger->Debug("World destroyed");
        }
    }

    void World::AddEntity(std::unique_ptr<Engine::Entity> entity) {
        if (!entity) {
            if (m_logger) {
                m_logger->Warning("Attempted to add null entity to world");
            }
            return;
        }
        
        Engine::Entity* raw = entity.get();
        uint32_t id = raw->GetId();
        m_entities.push_back(std::move(entity));
        m_entityList.push_back(raw);
        m_entityMap[id] = raw;
        m_spatialGrid.Insert(raw);

        // Track characters separately to avoid dynamic_cast during iteration
        if (auto* character = dynamic_cast<Entities::Character*>(raw)) {
            m_characters.push_back(character);
        }
        
        if (m_logger) {
            m_logger->Debug("Entity added to world. Total entities: " + 
                          std::to_string(m_entities.size()));
        }
    }

    void World::RemoveEntity(Engine::Entity* entity) {
        if (!entity) {
            return;
        }
        
        uint32_t id = entity->GetId();
        m_spatialGrid.Remove(entity);
        m_entityMap.erase(id);
        m_entityPositions.erase(id);

        // Remove from character list
        if (auto* character = dynamic_cast<Entities::Character*>(entity)) {
            auto cit = std::find(m_characters.begin(), m_characters.end(), character);
            if (cit != m_characters.end()) {
                // Swap-and-pop for O(1) removal
                *cit = m_characters.back();
                m_characters.pop_back();
            }
        }

        auto it = std::find_if(m_entities.begin(), m_entities.end(),
            [entity](const std::unique_ptr<Engine::Entity>& e) {
                return e.get() == entity;
            });
        
        if (it != m_entities.end()) {
            // Swap-and-pop for O(1) removal from entity list
            auto lit = std::find(m_entityList.begin(), m_entityList.end(), entity);
            if (lit != m_entityList.end()) {
                *lit = m_entityList.back();
                m_entityList.pop_back();
            }

            // Swap-and-pop from ownership vector
            *it = std::move(m_entities.back());
            m_entities.pop_back();
            
            if (m_logger) {
                m_logger->Debug("Entity removed from world. Remaining entities: " + 
                              std::to_string(m_entities.size()));
            }
        }
    }

    void World::ClearEntities() {
        m_spatialGrid.Clear();
        m_entities.clear();
        m_entityList.clear();
        m_entityMap.clear();
        m_characters.clear();
        m_entityPositions.clear();
        m_occupancy.clear();
        
        if (m_logger) {
            m_logger->Debug("World entities cleared");
        }
    }

    Engine::Entity* World::GetEntityById(uint32_t id) {
        auto it = m_entityMap.find(id);
        return (it != m_entityMap.end()) ? it->second : nullptr;
    }

    Entities::Character* World::GetCharacterById(uint32_t id) {
        Engine::Entity* entity = GetEntityById(id);
        return entity ? dynamic_cast<Entities::Character*>(entity) : nullptr;
    }

    Entities::Character* World::GetCharacterAtTile(const Engine::TilePosition& pos) {
        auto it = m_occupancy.find(pos);
        return (it != m_occupancy.end()) ? it->second : nullptr;
    }

    Entities::Character* World::GetCharacterAtTile(uint16_t row, uint16_t col) {
        return GetCharacterAtTile(Engine::TilePosition(row, col));
    }

    bool World::IsOccupied(const Engine::TilePosition& pos) const {
        return m_occupancy.find(pos) != m_occupancy.end();
    }

    void World::PlaceCharacter(Entities::Character* character, const Engine::TilePosition& pos) {
        if (!character) return;
        // O(1) removal from old position via reverse lookup
        uint32_t id = character->GetId();
        auto posIt = m_entityPositions.find(id);
        if (posIt != m_entityPositions.end()) {
            m_occupancy.erase(posIt->second);
        }
        m_occupancy[pos] = character;
        m_entityPositions[id] = pos;
        character->SetTilePosition(pos);
    }

    void World::RemoveOccupant(const Engine::TilePosition& pos) {
        m_occupancy.erase(pos);
    }

    void World::SetTileMap(Engine::TileMap* tileMap) {
        m_tileMap = tileMap;
    }

    void World::Update(float deltaTime) {
        // Update all entities
        for (auto& entity : m_entities) {
            entity->Update(deltaTime);
        }
    }

    std::vector<Engine::Entity*> World::GetEntitiesInRadius(const Engine::Point& center, float radius) {
        return m_spatialGrid.QueryRadius(center, radius);
    }

    std::vector<Engine::Entity*> World::GetEntitiesInRect(const Engine::Rect& rect) {
        return m_spatialGrid.QueryRect(rect);
    }


    }

} // namespace World
} // namespace LegalCrime
