#include "GameplayScene.h"
#include "../../Engine/World/TileMap.h"

namespace LegalCrime {

    // Private implementation to hide old game code dependencies
    class GameplaySceneImpl {
    public:
        std::unique_ptr<Engine::TileMap> tileMap;

        GameplaySceneImpl()
            : tileMap(nullptr) {
        }

        ~GameplaySceneImpl() {
            Cleanup();
        }

        void Cleanup() {
            tileMap.reset();
        }
    };

    GameplayScene::GameplayScene(Engine::ILogger* logger, Engine::IRenderer* renderer)
        : Scene("Gameplay", logger, renderer)
        , m_impl(std::make_unique<GameplaySceneImpl>()) {
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

        // Create and initialize new TileMap system (100x100 tiles)
        m_impl->tileMap = std::make_unique<Engine::TileMap>(100, 100, m_logger);

        // Initialize with default window size (800x600)
        // TODO: Get actual window dimensions from window manager/config
        auto initResult = m_impl->tileMap->Initialize(800, 600);
        if (!initResult) {
            return Engine::Result<void>::Failure("Failed to initialize TileMap: " + initResult.error);
        }

        m_initialized = true;

        if (m_logger) {
            m_logger->Info("Gameplay Scene initialized successfully with TileMap");
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

        m_impl->Cleanup();
        m_initialized = false;
    }

    void GameplayScene::HandleEvent(const SDL_Event& event) {
        if (!m_initialized || !m_impl->tileMap) {
            return;
        }

        // Forward events to TileMap for camera panning
        m_impl->tileMap->HandleEvent(event);
    }

    void GameplayScene::Update(float deltaTime) {
        if (!m_initialized || !m_impl->tileMap) {
            return;
        }

        // TileMap updates would go here if needed in the future
        // For now, TileMap is stateless between frames
    }

    void GameplayScene::Render() {
        if (!m_initialized || !m_impl->tileMap || !m_renderer) {
            return;
        }

        // Render the TileMap using the modern renderer interface
        m_impl->tileMap->Render(m_renderer);
    }
}
