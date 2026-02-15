#include "World.h"
#include "../../Engine/World/TileMap.h"
#include "../../Engine/Core/Logger/ILogger.h"
#include "../Entities/Character.h"
#include <algorithm>
#include <cmath>

namespace LegalCrime {
namespace World {

    World::World(Engine::ILogger* logger)
        : m_logger(logger)
        , m_tileMap(nullptr) {
        
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
        
        m_entities.push_back(std::move(entity));
        RebuildEntityList();
        
        if (m_logger) {
            m_logger->Debug("Entity added to world. Total entities: " + 
                          std::to_string(m_entities.size()));
        }
    }

    void World::RemoveEntity(Engine::Entity* entity) {
        if (!entity) {
            return;
        }
        
        auto it = std::find_if(m_entities.begin(), m_entities.end(),
            [entity](const std::unique_ptr<Engine::Entity>& e) {
                return e.get() == entity;
            });
        
        if (it != m_entities.end()) {
            m_entities.erase(it);
            RebuildEntityList();
            
            if (m_logger) {
                m_logger->Debug("Entity removed from world. Remaining entities: " + 
                              std::to_string(m_entities.size()));
            }
        }
    }

    void World::ClearEntities() {
        m_entities.clear();
        m_entityList.clear();
        
        if (m_logger) {
            m_logger->Debug("World entities cleared");
        }
    }

    Engine::Entity* World::GetEntityById(uint32_t id) {
        for (auto& entity : m_entities) {
            if (entity->GetId() == id) {
                return entity.get();
            }
        }
        return nullptr;
    }

    Entities::Character* World::GetCharacterById(uint32_t id) {
        Engine::Entity* entity = GetEntityById(id);
        return entity ? dynamic_cast<Entities::Character*>(entity) : nullptr;
    }

    std::vector<Entities::Character*> World::GetAllCharacters() {
        std::vector<Entities::Character*> characters;
        
        for (auto& entity : m_entities) {
            if (auto* character = dynamic_cast<Entities::Character*>(entity.get())) {
                characters.push_back(character);
            }
        }
        
        return characters;
    }

    Entities::Character* World::GetCharacterAtTile(uint16_t row, uint16_t col) {
        for (auto& entity : m_entities) {
            if (auto* character = dynamic_cast<Entities::Character*>(entity.get())) {
                if (character->GetTileRow() == row && character->GetTileCol() == col) {
                    return character;
                }
            }
        }
        return nullptr;
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
        std::vector<Engine::Entity*> result;
        float radiusSq = radius * radius;
        
        for (auto& entity : m_entities) {
            const Engine::Transform& transform = entity->GetTransform();
            float dx = static_cast<float>(transform.position.x - center.x);
            float dy = static_cast<float>(transform.position.y - center.y);
            float distSq = dx * dx + dy * dy;
            
            if (distSq <= radiusSq) {
                result.push_back(entity.get());
            }
        }
        
        return result;
    }

    void World::RebuildEntityList() {
        m_entityList.clear();
        m_entityList.reserve(m_entities.size());
        
        for (auto& entity : m_entities) {
            m_entityList.push_back(entity.get());
        }
    }

} // namespace World
} // namespace LegalCrime
