#include "GameplayScene.h"
#include "GameplayInputHandler.h"
#include "GameplayRenderer.h"
#include "../Simulation/GameSimulation.h"
#include "../../Engine/World/TileMap.h"
#include "../../Engine/World/TileMapRenderer.h"
#include "../../Engine/Camera/Camera2D.h"
#include "../World/Systems/SelectionSystem.h"
#include "../../Engine/Core/Constants.h"

namespace LegalCrime {

    GameplayScene::GameplayScene(
        Engine::ILogger* logger,
        Engine::IRenderer* renderer,
        Engine::Input::InputManager* inputManager,
        Engine::Resources::ResourceManager* resourceManager)
        : Scene("Gameplay", logger, renderer)
        , m_inputManager(inputManager) {
        m_simulation = std::make_unique<Simulation::GameSimulation>(logger, resourceManager);
        m_inputHandler = std::make_unique<GameplayInputHandler>(logger, inputManager);
        m_gameplayRenderer = std::make_unique<GameplayRenderer>(logger, renderer);
    }

    GameplayScene::~GameplayScene() {
        Shutdown();
    }

    Engine::Result<void> GameplayScene::Initialize() {
        if (m_initialized) {
            return Engine::Result<void>::Failure("GameplayScene already initialized");
        }

        if (!m_renderer) {
            return Engine::Result<void>::Failure("Renderer is null");
        }

        if (m_logger) {
            m_logger->Info("Initializing Gameplay Scene");
        }

        // Create and initialize new TileMap system (20x20 tiles)
        m_tileMap = std::make_unique<Engine::TileMap>(20, 20, m_logger);

        int windowWidth = Engine::Constants::Window::DEFAULT_WIDTH;
        int windowHeight = Engine::Constants::Window::DEFAULT_HEIGHT;
        if (m_renderer && m_renderer->GetNativeRenderer()) {
            int w = 0;
            int h = 0;
            if (SDL_GetCurrentRenderOutputSize(m_renderer->GetNativeRenderer(), &w, &h)) {
                if (w > 0 && h > 0) {
                    windowWidth = w;
                    windowHeight = h;
                }
            }
        }

        auto initResult = m_tileMap->Initialize(windowWidth, windowHeight);
        if (!initResult) {
            return Engine::Result<void>::Failure("Failed to initialize TileMap: " + initResult.error);
        }

        // Create tile map renderer
        m_tileMapRenderer = std::make_unique<Engine::TileMapRenderer>(m_logger);
        m_tileMapRenderer->Initialize(windowWidth, windowHeight, *m_tileMap);

        // Create camera
        m_camera = std::make_unique<Engine::Camera2D>(windowWidth, windowHeight, m_logger);

        // Set camera bounds to match map bounds
        Engine::Rect mapBounds = m_tileMap->GetBounds();
        m_camera->SetBounds(mapBounds);

        if (m_logger) {
            m_logger->Info("Camera created with bounds: (" + 
                         std::to_string(mapBounds.x) + ", " + std::to_string(mapBounds.y) + ", " +
                         std::to_string(mapBounds.w) + ", " + std::to_string(mapBounds.h) + ")");
        }

        // Create some obstacle tiles for testing pathfinding
        // Create a wall pattern
        for (uint16_t i = 5; i < 15; ++i) {
            Engine::Tile* tile = m_tileMap->GetTile(i, 10);
            if (tile) {
                tile->SetWalkable(false);
                tile->SetId(2); // Wall tile ID
            }
        }
        // Add a gap in the wall
        Engine::Tile* gapTile = m_tileMap->GetTile(10, 10);
        if (gapTile) {
            gapTile->SetWalkable(true);
            gapTile->SetId(1);
        }

        auto simInit = m_simulation->Initialize(m_tileMap.get());
        if (!simInit) {
            return Engine::Result<void>::Failure("Failed to initialize simulation: " + simInit.error);
        }

        m_initialized = true;

        if (m_logger) {
            m_logger->Info("Gameplay Scene initialized successfully with World/Systems architecture");
        }

        return Engine::Result<void>::Success();
    }

    void GameplayScene::Shutdown() {
        if (!m_initialized) {
            return;
        }

        if (m_logger) {
            m_logger->Info("Shutting down Gameplay Scene");
        }

        if (m_simulation) {
            m_simulation->Shutdown();
        }
        m_tileMapRenderer.reset();
        m_tileMap.reset();
        m_camera.reset();
        m_initialized = false;
    }

    void GameplayScene::HandleEvent(const SDL_Event& event) {
        if (!m_initialized || !m_tileMap || !m_inputManager) {
            return;
        }

        // Camera panning is handled via InputActions (CAMERA_PAN)
        // Selection is handled by SelectionSystem::Update()
    }

    void GameplayScene::Update(float deltaTime) {
        if (!m_initialized || !m_tileMap || !m_inputManager) {
            return;
        }

        // Update camera
        if (m_camera) {
            m_camera->Update(deltaTime);
        }

        if (m_simulation) {
            m_simulation->Update(deltaTime);
        }

        if (m_inputHandler) {
            m_inputHandler->HandleInputActions(m_simulation.get(), m_tileMap.get(), m_camera.get());
        }

        if (m_gameplayRenderer) {
            m_gameplayRenderer->SyncEntityScreenPositions(m_simulation.get(), m_tileMap.get(), m_camera.get());
        }
    }

    void GameplayScene::Render() {
        if (!m_initialized || !m_tileMap || !m_renderer) {
            return;
        }

        if (m_gameplayRenderer) {
            m_gameplayRenderer->Render(m_simulation.get(), m_tileMap.get(), m_tileMapRenderer.get(), m_camera.get());
        }
    }

    void GameplayScene::MoveCharacterToTile(const Engine::TilePosition& target, float duration) {
        if (!m_initialized || !m_simulation) {
            return;
        }
        m_simulation->MoveCharacterToTile(target, duration);
    }

    void GameplayScene::MoveCharacterToTile(uint16_t row, uint16_t col, float duration) {
        MoveCharacterToTile(Engine::TilePosition(row, col), duration);
    }

    void GameplayScene::SelectCharacterAt(int screenX, int screenY) {
        if (!m_initialized || !m_inputHandler) {
            return;
        }
        m_inputHandler->SelectCharacterAt(m_simulation.get(), screenX, screenY, m_tileMap.get(), m_camera.get());
    }

    void GameplayScene::CommandMoveToPosition(int screenX, int screenY) {
        if (!m_initialized || !m_inputHandler) {
            return;
        }
        m_inputHandler->CommandMoveToPosition(m_simulation.get(), screenX, screenY, m_tileMap.get(), m_camera.get());
    }
}
