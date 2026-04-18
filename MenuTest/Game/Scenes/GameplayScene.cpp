#include "GameplayScene.h"
#include "../../Engine/World/TileMap.h"
#include "../../Engine/World/TileMapRenderer.h"
#include "../../Engine/Camera/Camera2D.h"
#include "../Entities/Character.h"
#include "../Entities/CharacterFactory.h"
#include "../World/World.h"
#include "../World/Systems/MovementSystem.h"
#include "../World/Systems/SelectionSystem.h"
#include "../Input/GameInputBindings.h"
#include <cmath>

namespace LegalCrime {

    GameplayScene::GameplayScene(
        Engine::ILogger* logger,
        Engine::IRenderer* renderer,
        Engine::Input::InputManager* inputManager,
        Engine::Resources::ResourceManager* resourceManager)
        : Scene("Gameplay", logger, renderer)
        , m_inputManager(inputManager)
        , m_character(nullptr) {
        // Create character factory
        m_characterFactory = std::make_unique<Entities::CharacterFactory>(
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
        m_tileMap = std::make_unique<Engine::TileMap>(20, 20, m_logger);

        // Initialize with default window size (800x600)
        // TODO: Get actual window dimensions from window manager/config
        const int windowWidth = 800;
        const int windowHeight = 600;

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

        // Create World and Systems
        m_world = std::make_unique<World::World>(m_logger);
        m_world->SetTileMap(m_tileMap.get());

        m_movementSystem = std::make_unique<World::MovementSystem>(m_logger);
        m_movementSystem->Initialize(m_tileMap.get());

        m_selectionSystem = std::make_unique<World::SelectionSystem>(m_logger);

        // Create a thug character using the factory
        auto characterUnique = m_characterFactory->CreateCharacter(Entities::CharacterType::Thug);

        if (characterUnique) {
            // Place character at center of map
            uint16_t startRow = m_tileMap->GetHeight() / 2;
            uint16_t startCol = m_tileMap->GetWidth() / 2;
            Engine::TilePosition startPos(startRow, startCol);

            // Keep raw pointer for convenience
            m_character = characterUnique.get();

            // Transfer ownership to World
            m_world->AddEntity(std::move(characterUnique));

            // Place character via World occupancy management
            m_world->PlaceCharacter(m_character, startPos);

            // Convert tile position to screen coordinates
            Engine::Point screenPos = m_tileMap->TileToScreenCenter(startPos);

            // Offset character position to appear centered on tile
            // Subtract half the sprite size to center it
            int spriteWidth = static_cast<int>(50 * characterUnique->GetScale());
            int spriteHeight = static_cast<int>(50 * characterUnique->GetScale());
            screenPos.x -= spriteWidth / 2;
            screenPos.y -= spriteHeight / 2;

            // Offset character higher on the tile
            int tileHeightOffset = m_tileMap->GetTileHeight();
            screenPos.y -= tileHeightOffset;

            m_character->SetPosition(screenPos.x, screenPos.y);

            if (m_logger) {
                m_logger->Info("Character entity created successfully");
                m_logger->Info("Character placed at tile (" + std::to_string(startPos.row) + 
                             ", " + std::to_string(startPos.col) + ") -> screen (" + 
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

        m_character = nullptr;
        m_selectionSystem.reset();
        m_movementSystem.reset();
        m_world.reset();
        m_characterFactory.reset();
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

    void GameplayScene::HandleInputActions() {
        using namespace InputBindings;

        // Camera Pan (middle mouse button)
        if (m_inputManager->WasActionJustPressed(Actions::CAMERA_PAN)) {
            if (m_logger) {
                m_logger->Debug("Camera Pan PRESSED - starting pan");
            }
            Engine::Input::MousePosition mousePos = m_inputManager->GetMousePosition();
            m_camera->StartPan(mousePos.x, mousePos.y);
        } else if (m_inputManager->IsActionHeld(Actions::CAMERA_PAN)) {
            if (m_camera->IsPanning()) {
                Engine::Input::MousePosition mousePos = m_inputManager->GetMousePosition();
                m_camera->UpdatePan(mousePos.x, mousePos.y);
            }
        } else if (m_inputManager->WasActionJustReleased(Actions::CAMERA_PAN)) {
            if (m_logger) {
                m_logger->Debug("Camera Pan RELEASED - ending pan");
            }
            m_camera->EndPan();
        }

        // Selection system handles SELECT action internally
        // Command (right mouse button) - move selected character
        if (m_inputManager->WasActionJustPressed(Actions::COMMAND)) {
            if (m_selectionSystem->HasSelection()) {
                Engine::Input::MousePosition mousePos = m_inputManager->GetMousePosition();
                CommandMoveToPosition(mousePos.x, mousePos.y);
            }
        }

        // Handle keyboard movement (only when character exists, not moving, and not selected)
        if (m_character && 
            !m_movementSystem->IsCharacterMoving(m_character) &&
            !m_selectionSystem->HasSelection()) {

            uint16_t currentRow = m_character->GetTileRow();
            uint16_t currentCol = m_character->GetTileCol();
            uint16_t newRow = currentRow;
            uint16_t newCol = currentCol;
            bool moved = false;

            if (m_inputManager->WasActionJustPressed(Actions::MOVE_UP)) {
                if (currentRow > 0) {
                    newRow--;
                    moved = true;
                }
            } else if (m_inputManager->WasActionJustPressed(Actions::MOVE_DOWN)) {
                if (currentRow < m_tileMap->GetHeight() - 1) {
                    newRow++;
                    moved = true;
                }
            } else if (m_inputManager->WasActionJustPressed(Actions::MOVE_LEFT)) {
                if (currentCol > 0) {
                    newCol--;
                    moved = true;
                }
            } else if (m_inputManager->WasActionJustPressed(Actions::MOVE_RIGHT)) {
                if (currentCol < m_tileMap->GetWidth() - 1) {
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
        if (!m_initialized || !m_tileMap || !m_inputManager) {
            return;
        }

        // Update camera
        if (m_camera) {
            m_camera->Update(deltaTime);
        }

        // Update world (updates all entities)
        if (m_world) {
            m_world->Update(deltaTime);
        }

        // Update systems
        if (m_movementSystem) {
            m_movementSystem->Update(m_world.get(), deltaTime);
        }

        if (m_selectionSystem) {
            m_selectionSystem->Update(
                m_world.get(),
                m_inputManager,
                m_tileMap.get(),
                m_camera.get()
            );
        }

        // Handle input actions
        HandleInputActions();

        // Update character visual position to match tile position (from camera panning)
        if (m_character) {
            Engine::TilePosition charPos = m_character->GetTilePosition();

            Engine::Point screenPos = m_tileMap->TileToScreenCenter(
                charPos.row,
                charPos.col,
                m_camera.get()
            );

            // Center sprite on tile
            int spriteWidth = static_cast<int>(50 * m_character->GetScale());
            int spriteHeight = static_cast<int>(50 * m_character->GetScale());
            screenPos.x -= spriteWidth / 2;
            screenPos.y -= spriteHeight / 2;

            // Offset character higher on the tile
            int tileHeightOffset = m_tileMap->GetTileHeight();
            screenPos.y -= tileHeightOffset;

            m_character->SetPosition(screenPos.x, screenPos.y);
        }
    }

    void GameplayScene::Render() {
        if (!m_initialized || !m_tileMap || !m_renderer) {
            return;
        }

        // Render the TileMap using the renderer
        m_tileMapRenderer->Render(m_renderer, *m_tileMap, m_camera.get());

        // Render all entities in the world
        if (m_world) {
            for (Engine::Entity* entity : m_world->GetAllEntities()) {
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
        if (m_selectionSystem && m_selectionSystem->HasSelection()) {
            Entities::Character* selectedChar = m_selectionSystem->GetSelectedCharacter();
            if (selectedChar) {
                // Get character center position on screen
                Engine::Point tileCenter = m_tileMap->TileToScreenCenter(
                    selectedChar->GetTilePosition().row,
                    selectedChar->GetTilePosition().col,
                    m_camera.get()
                );

                // Draw selection circle around character
                m_renderer->SetDrawColor(0, 255, 0, 255);

                const int radius = 40;
                const int segments = 32;
                Engine::Point points[segments + 1];

                for (int i = 0; i <= segments; ++i) {
                    float angle = (i * 2.0f * 3.14159f) / segments;
                    points[i].x = static_cast<int>(tileCenter.x + radius * cos(angle));
                    points[i].y = static_cast<int>(tileCenter.y + radius * sin(angle));
                }

                m_renderer->DrawLines(points, segments + 1);
            }
        }
    }

    void GameplayScene::MoveCharacterToTile(const Engine::TilePosition& target, float duration) {
        if (!m_initialized || !m_character || !m_tileMap || !m_movementSystem) {
            return;
        }

        // Validate tile coordinates
        if (target.row >= m_tileMap->GetHeight() || target.col >= m_tileMap->GetWidth()) {
            if (m_logger) {
                m_logger->Warning("Invalid tile coordinates: (" + std::to_string(target.row) + 
                                ", " + std::to_string(target.col) + ")");
            }
            return;
        }

        // Use MovementSystem to handle the movement
        m_movementSystem->MoveCharacterToTile(m_character, target, duration);
    }

    void GameplayScene::MoveCharacterToTile(uint16_t row, uint16_t col, float duration) {
        MoveCharacterToTile(Engine::TilePosition(row, col), duration);
    }

    void GameplayScene::SelectCharacterAt(int screenX, int screenY) {
        if (!m_initialized || !m_selectionSystem || !m_world) {
            return;
        }

        // SelectionSystem handles the logic
        Entities::Character* character = m_selectionSystem->GetCharacterAtScreenPosition(
            m_world.get(),
            screenX,
            screenY,
            m_tileMap.get(),
            m_camera.get()
        );

        if (character) {
            m_selectionSystem->SelectCharacter(character);
        } else {
            m_selectionSystem->ClearSelection();
        }
    }

    void GameplayScene::CommandMoveToPosition(int screenX, int screenY) {
        if (!m_initialized || !m_tileMap || !m_movementSystem || !m_selectionSystem) {
            return;
        }

        Entities::Character* selectedChar = m_selectionSystem->GetSelectedCharacter();
        if (!selectedChar) {
            return;
        }

        // Convert screen coordinates to tile
        uint16_t targetRow, targetCol;
        if (!m_tileMap->ScreenToTile(screenX, screenY, targetRow, targetCol, m_camera.get())) {
            if (m_logger) {
                m_logger->Warning("Right-click outside map bounds");
            }
            return;
        }

        // Don't move if clicking on the exact same tile the character is on
        Engine::TilePosition targetPos(targetRow, targetCol);
        if (targetPos == selectedChar->GetTilePosition()) {
            if (m_logger) {
                m_logger->Debug("Right-clicked on character's current tile - no movement");
            }
            return;
        }

        // Check if target tile is walkable
        const Engine::Tile* targetTile = m_tileMap->GetTile(targetRow, targetCol);
        if (!targetTile || !targetTile->IsWalkable()) {
            if (m_logger) {
                m_logger->Warning("Cannot move to obstacle tile (" + std::to_string(targetRow) + 
                                ", " + std::to_string(targetCol) + ")");
            }
            return;
        }

        // Use MovementSystem to handle pathfinding and movement
        m_movementSystem->MoveCharacterToTile(selectedChar, targetPos);
    }
}
