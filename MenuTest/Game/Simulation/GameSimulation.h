#pragma once

#include "../../Engine/Core/Types.h"
#include <memory>

namespace Engine {
    class ILogger;
    class TileMap;
    namespace Resources {
        class ResourceManager;
    }
}

namespace LegalCrime {
namespace Entities {
    class Character;
    class CharacterFactory;
}
namespace World {
    class World;
    class MovementSystem;
    class SelectionSystem;
    class CommandSystem;
}
namespace Simulation {

    // Owns and updates the gameplay simulation state (world + systems).
    class GameSimulation {
    public:
        GameSimulation(Engine::ILogger* logger, Engine::Resources::ResourceManager* resourceManager);
        ~GameSimulation();

        Engine::Result<void> Initialize(Engine::TileMap* tileMap);
        void Shutdown();
        void Update(float deltaTime);

        bool IsInitialized() const { return m_initialized; }

        World::World* GetWorld() { return m_world.get(); }
        World::MovementSystem* GetMovementSystem() { return m_movementSystem.get(); }
        World::SelectionSystem* GetSelectionSystem() { return m_selectionSystem.get(); }
        World::CommandSystem* GetCommandSystem() { return m_commandSystem.get(); }
        Entities::Character* GetPrimaryCharacter() const { return m_primaryCharacter; }

        bool MoveCharacterToTile(const Engine::TilePosition& target, float duration = 0.3f);

    private:
        Engine::ILogger* m_logger;
        Engine::Resources::ResourceManager* m_resourceManager;
        bool m_initialized;

        std::unique_ptr<Entities::CharacterFactory> m_characterFactory;
        std::unique_ptr<World::World> m_world;
        std::unique_ptr<World::MovementSystem> m_movementSystem;
        std::unique_ptr<World::SelectionSystem> m_selectionSystem;
        std::unique_ptr<World::CommandSystem> m_commandSystem;

        // Convenience pointer to spawned starter unit, owned by World.
        Entities::Character* m_primaryCharacter;
    };

} // namespace Simulation
} // namespace LegalCrime
