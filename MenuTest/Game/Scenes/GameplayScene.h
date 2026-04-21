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

    class GameplayInputHandler;
    class GameplayRenderer;

    namespace Simulation {
        class GameSimulation;
    }

    namespace Entities {
        class Character;
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

        Engine::SceneId GetSceneId() const override { return Engine::SceneId::Gameplay; }

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
        std::unique_ptr<Simulation::GameSimulation> m_simulation;
        std::unique_ptr<GameplayInputHandler> m_inputHandler;
        std::unique_ptr<GameplayRenderer> m_gameplayRenderer;
        Engine::Input::InputManager* m_inputManager;
    };
}
