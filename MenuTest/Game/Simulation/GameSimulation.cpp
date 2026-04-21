#include "GameSimulation.h"
#include "../Entities/Character.h"
#include "../Entities/CharacterFactory.h"
#include "../World/World.h"
#include "../World/Systems/MovementSystem.h"
#include "../World/Systems/SelectionSystem.h"
#include "../World/Systems/CommandSystem.h"
#include "../../Engine/World/TileMap.h"
#include "../../Engine/Core/Logger/ILogger.h"
#include "../../Engine/Resources/ResourceManager.h"

namespace LegalCrime {
namespace Simulation {

    GameSimulation::GameSimulation(Engine::ILogger* logger, Engine::Resources::ResourceManager* resourceManager)
        : m_logger(logger)
        , m_resourceManager(resourceManager)
        , m_initialized(false)
        , m_primaryCharacter(nullptr) {
    }

    GameSimulation::~GameSimulation() {
        Shutdown();
    }

    Engine::Result<void> GameSimulation::Initialize(Engine::TileMap* tileMap) {
        if (m_initialized) {
            return Engine::Result<void>::Failure("GameSimulation already initialized");
        }

        if (!tileMap) {
            return Engine::Result<void>::Failure("TileMap is null");
        }

        if (m_resourceManager) {
            m_characterFactory = std::make_unique<Entities::CharacterFactory>(m_resourceManager, m_logger);
        }

        uint16_t worldPixelW = tileMap->GetMapSizeWidth();
        uint16_t worldPixelH = tileMap->GetMapSizeHeight();

        m_world = std::make_unique<World::World>(worldPixelW, worldPixelH, 64, m_logger);
        m_world->SetTileMap(tileMap);

        m_movementSystem = std::make_unique<World::MovementSystem>(m_logger);
        m_movementSystem->Initialize(tileMap);

        m_selectionSystem = std::make_unique<World::SelectionSystem>(m_logger);
        m_commandSystem = std::make_unique<World::CommandSystem>(m_logger);

        // Spawn a starter character to keep the existing gameplay behavior.
        if (m_characterFactory) {
            auto characterUnique = m_characterFactory->CreateCharacter(Entities::CharacterType::Thug);
            if (characterUnique) {
                uint16_t startRow = tileMap->GetHeight() / 2;
                uint16_t startCol = tileMap->GetWidth() / 2;
                Engine::TilePosition startPos(startRow, startCol);

                m_primaryCharacter = m_world->SpawnCharacter(std::move(characterUnique), startPos);

                if (m_logger) {
                    m_logger->Info("GameSimulation: starter character spawned");
                }
            }
        }

        m_initialized = true;
        return Engine::Result<void>::Success();
    }

    void GameSimulation::Shutdown() {
        if (!m_initialized) {
            return;
        }

        m_primaryCharacter = nullptr;
        m_commandSystem.reset();
        m_selectionSystem.reset();
        m_movementSystem.reset();
        m_world.reset();
        m_characterFactory.reset();
        m_initialized = false;
    }

    void GameSimulation::Update(float deltaTime) {
        if (!m_initialized) {
            return;
        }

        if (m_world) {
            m_world->Update(deltaTime);
        }

        if (m_movementSystem) {
            m_movementSystem->Update(m_world.get(), deltaTime);
        }

        if (m_commandSystem) {
            m_commandSystem->Update(m_world.get(), m_movementSystem.get(), deltaTime);
        }
    }

    bool GameSimulation::MoveCharacterToTile(const Engine::TilePosition& target, float duration) {
        if (!m_initialized || !m_primaryCharacter || !m_world || !m_movementSystem) {
            return false;
        }

        Engine::TileMap* tileMap = m_world->GetTileMap();
        if (!tileMap) {
            return false;
        }

        if (target.row >= tileMap->GetHeight() || target.col >= tileMap->GetWidth()) {
            if (m_logger) {
                m_logger->Warning("GameSimulation: invalid tile coordinates");
            }
            return false;
        }

        return m_movementSystem->MoveCharacterToTile(m_primaryCharacter, target, duration);
    }

} // namespace Simulation
} // namespace LegalCrime
