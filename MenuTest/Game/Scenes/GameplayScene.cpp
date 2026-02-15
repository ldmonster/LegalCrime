#include "GameplayScene.h"
#include "../../Engine/World/TileMap.h"
#include "../../Engine/Camera/Camera2D.h"
#include "../Entities/Character.h"
#include "../Entities/CharacterFactory.h"
#include "../World/World.h"
#include "../World/Systems/MovementSystem.h"
#include "../World/Systems/SelectionSystem.h"
#include "../Input/GameInputBindings.h"
#include <cmath>

namespace LegalCrime {

    // Private implementation using new World/Systems architecture
    class GameplaySceneImpl {
    public:
        std::unique_ptr<Engine::TileMap> tileMap;
        std::unique_ptr<Engine::Camera2D> camera;
        std::unique_ptr<Entities::CharacterFactory> characterFactory;
        std::unique_ptr<World::World> world;
        std::unique_ptr<World::MovementSystem> movementSystem;
        std::unique_ptr<World::SelectionSystem> selectionSystem;
        Engine::Input::InputManager* inputManager;

        // Raw pointer to character for convenience (owned by World)
        Entities::Character* character;

        GameplaySceneImpl()
            : tileMap(nullptr)
            , camera(nullptr)
            , characterFactory(nullptr)
            , world(nullptr)
            , movementSystem(nullptr)
            , selectionSystem(nullptr)
            , inputManager(nullptr)
            , character(nullptr) {
        }

        ~GameplaySceneImpl() {
            Cleanup();
        }

        void Cleanup() {
            character = nullptr;  // Owned by World
            selectionSystem.reset();
            movementSystem.reset();
            world.reset();
            characterFactory.reset();
            tileMap.reset();
            camera.reset();
        }
    };

    GameplayScene::GameplayScene(
        Engine::ILogger* logger,
        Engine::IRenderer* renderer,
        Engine::Input::InputManager* inputManager,
        Engine::Resources::ResourceManager* resourceManager)
        : Scene("Gameplay", logger, renderer)
        , m_impl(std::make_unique<GameplaySceneImpl>()) {
        // Store input manager reference
        m_impl->inputManager = inputManager;

        // Create character factory
        m_impl->characterFactory = std::make_unique<Entities::CharacterFactory>(
            resourceManager,
            logger
        );
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
        m_impl->tileMap = std::make_unique<Engine::TileMap>(20, 20, m_logger);

        // Initialize with default window size (800x600)
        // TODO: Get actual window dimensions from window manager/config
        const int windowWidth = 800;
        const int windowHeight = 600;

        auto initResult = m_impl->tileMap->Initialize(windowWidth, windowHeight);
        if (!initResult) {
            return Engine::Result<void>::Failure("Failed to initialize TileMap: " + initResult.error);
        }

        // Create camera
        m_impl->camera = std::make_unique<Engine::Camera2D>(windowWidth, windowHeight, m_logger);

        // Set camera bounds to match map bounds
        Engine::Rect mapBounds = m_impl->tileMap->GetBounds();
        m_impl->camera->SetBounds(mapBounds);

        if (m_logger) {
            m_logger->Info("Camera created with bounds: (" + 
                         std::to_string(mapBounds.x) + ", " + std::to_string(mapBounds.y) + ", " +
                         std::to_string(mapBounds.w) + ", " + std::to_string(mapBounds.h) + ")");
        }

        // Create some obstacle tiles for testing pathfinding
        // Create a wall pattern
        for (uint16_t i = 5; i < 15; ++i) {
            Engine::Tile* tile = m_impl->tileMap->GetTile(i, 10);
            if (tile) {
                tile->SetWalkable(false);
                tile->SetId(2); // Wall tile ID
            }
        }
        // Add a gap in the wall
        Engine::Tile* gapTile = m_impl->tileMap->GetTile(10, 10);
        if (gapTile) {
            gapTile->SetWalkable(true);
            gapTile->SetId(1);
        }

        // Create World and Systems
        m_impl->world = std::make_unique<World::World>(m_logger);
        m_impl->world->SetTileMap(m_impl->tileMap.get());

        m_impl->movementSystem = std::make_unique<World::MovementSystem>(m_logger);
        m_impl->movementSystem->Initialize(m_impl->tileMap.get());

        m_impl->selectionSystem = std::make_unique<World::SelectionSystem>(m_logger);

        // Create a thug character using the factory
        auto characterUnique = m_impl->characterFactory->CreateCharacter(Entities::CharacterType::Thug);

        if (characterUnique) {
            // Place character at center of map
            uint16_t startRow = m_impl->tileMap->GetHeight() / 2;
            uint16_t startCol = m_impl->tileMap->GetWidth() / 2;

            // Set tile position on character
            characterUnique->SetTilePosition(startRow, startCol);

            // Convert tile position to screen coordinates
            Engine::Point screenPos = m_impl->tileMap->TileToScreenCenter(
                startRow, 
                startCol
            );

            // Offset character position to appear centered on tile
            // Subtract half the sprite size to center it
            int spriteWidth = static_cast<int>(50 * characterUnique->GetScale());
            int spriteHeight = static_cast<int>(50 * characterUnique->GetScale());
            screenPos.x -= spriteWidth / 2;
            screenPos.y -= spriteHeight / 2;

            // Offset character higher on the tile
            int tileHeightOffset = m_impl->tileMap->GetTileHeight();
            screenPos.y -= tileHeightOffset;

            characterUnique->SetPosition(screenPos.x, screenPos.y);

            // Keep raw pointer for convenience
            m_impl->character = characterUnique.get();

            // Transfer ownership to World
            m_impl->world->AddEntity(std::move(characterUnique));

            if (m_logger) {
                m_logger->Info("Character entity created successfully");
                m_logger->Info("Character placed at tile (" + std::to_string(startRow) + 
                             ", " + std::to_string(startCol) + ") -> screen (" + 
                             std::to_string(screenPos.x) + ", " + std::to_string(screenPos.y) + ")");
            }
        } else {
            if (m_logger) {
                m_logger->Warning("Failed to create character entity");
            }
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

        m_impl->Cleanup();
        m_initialized = false;
    }

    void GameplayScene::HandleEvent(const SDL_Event& event) {
        if (!m_initialized || !m_impl->tileMap || !m_impl->inputManager) {
            return;
        }

        // Forward drag events to TileMap for camera panning
        // Only forward if not clicking on character
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
            // Check if clicking on character first
            Entities::Character* clickedChar = m_impl->selectionSystem->GetCharacterAtScreenPosition(
                m_impl->world.get(),
                (int)event.button.x,
                (int)event.button.y,
                m_impl->tileMap.get(),
                m_impl->camera.get()
            );

            // If not clicking on character, allow camera drag
            if (!clickedChar) {
                m_impl->tileMap->HandleEvent(event, m_impl->camera.get());
            }
        } else if (event.type != SDL_EVENT_MOUSE_BUTTON_DOWN || event.button.button != SDL_BUTTON_LEFT) {
            // Forward all other events normally
            m_impl->tileMap->HandleEvent(event, m_impl->camera.get());
        }
    }

    void GameplayScene::HandleInputActions() {
        using namespace InputBindings;

        // Selection system handles SELECT action internally
        // Command (right mouse button) - move selected character
        if (m_impl->inputManager->WasActionJustPressed(Actions::COMMAND)) {
            if (m_impl->selectionSystem->HasSelection()) {
                Engine::Input::MousePosition mousePos = m_impl->inputManager->GetMousePosition();
                CommandMoveToPosition(mousePos.x, mousePos.y);
            }
        }

        // Handle keyboard movement (only when character exists, not moving, and not selected)
        if (m_impl->character && 
            !m_impl->movementSystem->IsCharacterMoving(m_impl->character) &&
            !m_impl->selectionSystem->HasSelection()) {

            uint16_t currentRow = m_impl->character->GetTileRow();
            uint16_t currentCol = m_impl->character->GetTileCol();
            uint16_t newRow = currentRow;
            uint16_t newCol = currentCol;
            bool moved = false;

            if (m_impl->inputManager->WasActionJustPressed(Actions::MOVE_UP)) {
                if (currentRow > 0) {
                    newRow--;
                    moved = true;
                }
            } else if (m_impl->inputManager->WasActionJustPressed(Actions::MOVE_DOWN)) {
                if (currentRow < m_impl->tileMap->GetHeight() - 1) {
                    newRow++;
                    moved = true;
                }
            } else if (m_impl->inputManager->WasActionJustPressed(Actions::MOVE_LEFT)) {
                if (currentCol > 0) {
                    newCol--;
                    moved = true;
                }
            } else if (m_impl->inputManager->WasActionJustPressed(Actions::MOVE_RIGHT)) {
                if (currentCol < m_impl->tileMap->GetWidth() - 1) {
                    newCol++;
                    moved = true;
                }
            }

            if (moved) {
                MoveCharacterToTile(newRow, newCol, 0.3f);
            }
        }
    }

    void GameplayScene::Update(float deltaTime) {
        if (!m_initialized || !m_impl->tileMap || !m_impl->inputManager) {
            return;
        }

        // Update camera
        if (m_impl->camera) {
            m_impl->camera->Update(deltaTime);
        }

        // Update world (updates all entities)
        if (m_impl->world) {
            m_impl->world->Update(deltaTime);
        }

        // Update systems
        if (m_impl->movementSystem) {
            m_impl->movementSystem->Update(m_impl->world.get(), deltaTime);
        }

        if (m_impl->selectionSystem) {
            m_impl->selectionSystem->Update(
                m_impl->world.get(),
                m_impl->inputManager,
                m_impl->tileMap.get(),
                m_impl->camera.get()
            );
        }

        // Handle input actions
        HandleInputActions();

        // Update character visual position to match tile position (from camera panning)
        if (m_impl->character) {
            uint16_t charRow = m_impl->character->GetTileRow();
            uint16_t charCol = m_impl->character->GetTileCol();

            Engine::Point screenPos = m_impl->tileMap->TileToScreenCenter(
                charRow,
                charCol,
                m_impl->camera.get()
            );

            // Center sprite on tile
            int spriteWidth = static_cast<int>(50 * m_impl->character->GetScale());
            int spriteHeight = static_cast<int>(50 * m_impl->character->GetScale());
            screenPos.x -= spriteWidth / 2;
            screenPos.y -= spriteHeight / 2;

            // Offset character higher on the tile
            int tileHeightOffset = m_impl->tileMap->GetTileHeight();
            screenPos.y -= tileHeightOffset;

            m_impl->character->SetPosition(screenPos.x, screenPos.y);
        }
    }

    void GameplayScene::Render() {
        if (!m_initialized || !m_impl->tileMap || !m_renderer) {
            return;
        }

        // Render the TileMap using the camera
        m_impl->tileMap->Render(m_renderer, m_impl->camera.get());

        // Render all entities in the world
        if (m_impl->world) {
            for (Engine::Entity* entity : m_impl->world->GetAllEntities()) {
                if (entity) {
                    // Cast to Character to call Render(IRenderer*)
                    Entities::Character* character = dynamic_cast<Entities::Character*>(entity);
                    if (character) {
                        character->Render(m_renderer);
                    }
                }
            }
        }

        // Render selection indicator if character is selected
        if (m_impl->selectionSystem && m_impl->selectionSystem->HasSelection()) {
            Entities::Character* selectedChar = m_impl->selectionSystem->GetSelectedCharacter();
            if (selectedChar) {
                SDL_Renderer* sdlRenderer = m_renderer->GetNativeRenderer();
                if (sdlRenderer) {
                    // Get character center position on screen
                    Engine::Point tileCenter = m_impl->tileMap->TileToScreenCenter(
                        selectedChar->GetTileRow(),
                        selectedChar->GetTileCol(),
                        m_impl->camera.get()
                    );

                    // Draw selection circle around character
                    SDL_SetRenderDrawColor(sdlRenderer, 0, 255, 0, 255);  // Green

                    // Draw a circle using line segments
                    const int radius = 40;
                    const int segments = 32;
                    SDL_FPoint points[segments + 1];

                    for (int i = 0; i <= segments; ++i) {
                        float angle = (i * 2.0f * 3.14159f) / segments;
                        points[i].x = tileCenter.x + radius * cos(angle);
                        points[i].y = tileCenter.y + radius * sin(angle);
                    }

                    SDL_RenderLines(sdlRenderer, points, segments + 1);
                }
            }
        }
    }

    void GameplayScene::MoveCharacterToTile(uint16_t row, uint16_t col, float duration) {
        if (!m_initialized || !m_impl->character || !m_impl->tileMap || !m_impl->movementSystem) {
            return;
        }

        // Validate tile coordinates
        if (row >= m_impl->tileMap->GetHeight() || col >= m_impl->tileMap->GetWidth()) {
            if (m_logger) {
                m_logger->Warning("Invalid tile coordinates: (" + std::to_string(row) + 
                                ", " + std::to_string(col) + ")");
            }
            return;
        }

        // Use MovementSystem to handle the movement
        m_impl->movementSystem->MoveCharacterToTile(m_impl->character, row, col, duration);
    }

    void GameplayScene::SelectCharacterAt(int screenX, int screenY) {
        if (!m_initialized || !m_impl->selectionSystem || !m_impl->world) {
            return;
        }

        // SelectionSystem handles the logic
        Entities::Character* character = m_impl->selectionSystem->GetCharacterAtScreenPosition(
            m_impl->world.get(),
            screenX,
            screenY,
            m_impl->tileMap.get(),
            m_impl->camera.get()
        );

        if (character) {
            m_impl->selectionSystem->SelectCharacter(character);
        } else {
            m_impl->selectionSystem->ClearSelection();
        }
    }

    void GameplayScene::CommandMoveToPosition(int screenX, int screenY) {
        if (!m_initialized || !m_impl->tileMap || !m_impl->movementSystem || !m_impl->selectionSystem) {
            return;
        }

        Entities::Character* selectedChar = m_impl->selectionSystem->GetSelectedCharacter();
        if (!selectedChar) {
            return;
        }

        // Convert screen coordinates to tile
        uint16_t targetRow, targetCol;
        if (!m_impl->tileMap->ScreenToTile(screenX, screenY, targetRow, targetCol, m_impl->camera.get())) {
            if (m_logger) {
                m_logger->Warning("Right-click outside map bounds");
            }
            return;
        }

        // Don't move if clicking on the exact same tile the character is on
        if (targetRow == selectedChar->GetTileRow() && targetCol == selectedChar->GetTileCol()) {
            if (m_logger) {
                m_logger->Debug("Right-clicked on character's current tile - no movement");
            }
            return;
        }

        // Check if target tile is walkable
        const Engine::Tile* targetTile = m_impl->tileMap->GetTile(targetRow, targetCol);
        if (!targetTile || !targetTile->IsWalkable()) {
            if (m_logger) {
                m_logger->Warning("Cannot move to obstacle tile (" + std::to_string(targetRow) + 
                                ", " + std::to_string(targetCol) + ")");
            }
            return;
        }

        // Use MovementSystem to handle pathfinding and movement
        m_impl->movementSystem->MoveCharacterToTile(selectedChar, targetRow, targetCol);
    }
}
