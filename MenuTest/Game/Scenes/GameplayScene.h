#pragma once

#include "../../Engine/Scene/Scene.h"
#include "../../Engine/Core/Logger/ILogger.h"
#include "../../Engine/Renderer/IRenderer.h"
#include "../GameConstants.h"
#include <SDL3/SDL.h>
#include <memory>

// Forward declarations
namespace Engine {
    class TileMap;
    class TileMapRenderer;
    class Camera2D;
    namespace Input {
        class InputManager;
    }
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

    // Gameplay scene
    class GameplayScene : public Engine::Scene {
    public:
        GameplayScene(
            Engine::ILogger* logger,
            Engine::IRenderer* renderer,
            Engine::Input::InputManager* inputManager,
            Engine::Resources::ResourceManager* resourceManager);
        ~GameplayScene() override;

        Engine::Result<void> Initialize() override;
        void Shutdown() override;

        void HandleEvent(const SDL_Event& event) override;
        void Update(float deltaTime) override;
        void Render() override;

        // Character movement
        void MoveCharacterToTile(const Engine::TilePosition& target, float duration = Constants::Movement::DEFAULT_MOVE_DURATION);
        void MoveCharacterToTile(uint16_t row, uint16_t col, float duration = Constants::Movement::DEFAULT_MOVE_DURATION);

        // RTS-style commands
        void SelectCharacterAt(int screenX, int screenY);
        void CommandMoveToPosition(int screenX, int screenY);

    private:
        std::unique_ptr<Engine::TileMap> m_tileMap;
        std::unique_ptr<Engine::TileMapRenderer> m_tileMapRenderer;
        std::unique_ptr<Engine::Camera2D> m_camera;
        std::unique_ptr<Entities::CharacterFactory> m_characterFactory;
        std::unique_ptr<World::World> m_world;
        std::unique_ptr<World::MovementSystem> m_movementSystem;
        std::unique_ptr<World::SelectionSystem> m_selectionSystem;
        std::unique_ptr<World::CommandSystem> m_commandSystem;
        Engine::Input::InputManager* m_inputManager;

        // Raw pointer to character for convenience (owned by World)
        Entities::Character* m_character;

        // Input handling
        void HandleInputActions();
    };
}
