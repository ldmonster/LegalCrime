#include "GameplayInputHandler.h"
#include "../Simulation/GameSimulation.h"
#include "../World/World.h"
#include "../World/Systems/MovementSystem.h"
#include "../World/Systems/SelectionSystem.h"
#include "../World/Systems/CommandSystem.h"
#include "../World/Commands/Command.h"
#include "../Entities/Character.h"
#include "../Input/GameInputBindings.h"
#include "../../Engine/World/TileMap.h"
#include "../../Engine/Camera/Camera2D.h"
#include "../../Engine/Input/InputManager.h"
#include "../../Engine/Core/Logger/ILogger.h"

namespace LegalCrime {

    GameplayInputHandler::GameplayInputHandler(Engine::ILogger* logger, Engine::Input::InputManager* inputManager)
        : m_logger(logger)
        , m_inputManager(inputManager) {
    }

    void GameplayInputHandler::HandleInputActions(
        Simulation::GameSimulation* simulation,
        Engine::TileMap* tileMap,
        Engine::Camera2D* camera) {

        if (!simulation || !tileMap || !camera || !m_inputManager) {
            return;
        }

        using namespace InputBindings;

        auto* selectionSystem = simulation->GetSelectionSystem();
        auto* movementSystem = simulation->GetMovementSystem();
        auto* world = simulation->GetWorld();
        auto* primaryCharacter = simulation->GetPrimaryCharacter();

        Engine::Input::MousePosition mousePos = m_inputManager->GetMousePosition();
        if (selectionSystem) {
            selectionSystem->PruneSelectionToWorld(world);
            selectionSystem->UpdateHoveredTile(mousePos.x, mousePos.y, tileMap, camera);

            if (m_inputManager->IsMouseButtonPressed(Engine::Input::MouseButton::Left)) {
                if (!selectionSystem->IsBoxSelecting()) {
                    selectionSystem->StartBoxSelection(Engine::Point(mousePos.x, mousePos.y));
                } else {
                    selectionSystem->UpdateBoxSelection(Engine::Point(mousePos.x, mousePos.y));
                }
            } else if (selectionSystem->IsBoxSelecting()) {
                selectionSystem->EndBoxSelection(
                    world,
                    tileMap,
                    camera,
                    m_inputManager->IsShiftHeld()
                );
            }

            if (!selectionSystem->IsBoxSelecting() && m_inputManager->WasActionJustPressed(Actions::SELECT)) {
                selectionSystem->SelectAtScreenPosition(
                    world,
                    mousePos.x,
                    mousePos.y,
                    tileMap,
                    camera,
                    m_inputManager->IsShiftHeld(),
                    !m_inputManager->IsShiftHeld()
                );
            }

            // Control groups (0..9): ctrl+N saves, N recalls.
            for (int i = 0; i < selectionSystem->NUM_GROUPS; ++i) {
                SDL_Keycode key = SDLK_0 + i;
                if (m_inputManager->IsKeyPressed(key)) {
                    if (m_inputManager->IsCtrlHeld()) {
                        selectionSystem->SaveGroup(i);
                    } else {
                        selectionSystem->RecallGroup(i);
                    }
                }
            }
        }

        // Camera Pan (middle mouse button)
        if (m_inputManager->WasActionJustPressed(Actions::CAMERA_PAN)) {
            camera->StartPan(mousePos.x, mousePos.y);
        } else if (m_inputManager->IsActionHeld(Actions::CAMERA_PAN)) {
            if (camera->IsPanning()) {
                camera->UpdatePan(mousePos.x, mousePos.y);
            }
        } else if (m_inputManager->WasActionJustReleased(Actions::CAMERA_PAN)) {
            camera->EndPan();
        }

        // Command (right mouse button) - move selected character(s)
        if (m_inputManager->WasActionJustPressed(Actions::COMMAND) && selectionSystem && selectionSystem->HasSelection()) {
            CommandMoveToPosition(simulation, mousePos.x, mousePos.y, tileMap, camera);
        }

        // Keyboard movement for starter character when nothing is selected
        if (primaryCharacter && movementSystem && selectionSystem &&
            !movementSystem->IsCharacterMoving(primaryCharacter) &&
            !selectionSystem->HasSelection()) {

            uint16_t currentRow = primaryCharacter->GetTileRow();
            uint16_t currentCol = primaryCharacter->GetTileCol();
            uint16_t newRow = currentRow;
            uint16_t newCol = currentCol;
            bool moved = false;

            if (m_inputManager->WasActionJustPressed(Actions::MOVE_UP)) {
                if (currentRow > 0) {
                    --newRow;
                    moved = true;
                }
            } else if (m_inputManager->WasActionJustPressed(Actions::MOVE_DOWN)) {
                if (currentRow < tileMap->GetHeight() - 1) {
                    ++newRow;
                    moved = true;
                }
            } else if (m_inputManager->WasActionJustPressed(Actions::MOVE_LEFT)) {
                if (currentCol > 0) {
                    --newCol;
                    moved = true;
                }
            } else if (m_inputManager->WasActionJustPressed(Actions::MOVE_RIGHT)) {
                if (currentCol < tileMap->GetWidth() - 1) {
                    ++newCol;
                    moved = true;
                }
            }

            if (moved) {
                simulation->MoveCharacterToTile(Engine::TilePosition(newRow, newCol), 0.3f);
            }
        }
    }

    void GameplayInputHandler::SelectCharacterAt(
        Simulation::GameSimulation* simulation,
        int screenX,
        int screenY,
        Engine::TileMap* tileMap,
        Engine::Camera2D* camera) {

        if (!simulation || !tileMap) {
            return;
        }

        auto* world = simulation->GetWorld();
        auto* selectionSystem = simulation->GetSelectionSystem();
        if (!world || !selectionSystem) {
            return;
        }

        Entities::Character* character = selectionSystem->GetCharacterAtScreenPosition(
            world,
            screenX,
            screenY,
            tileMap,
            camera
        );

        if (character) {
            selectionSystem->SelectCharacter(character);
        } else {
            selectionSystem->ClearSelection();
        }
    }

    void GameplayInputHandler::CommandMoveToPosition(
        Simulation::GameSimulation* simulation,
        int screenX,
        int screenY,
        Engine::TileMap* tileMap,
        Engine::Camera2D* camera) {

        if (!simulation || !tileMap || !m_inputManager) {
            return;
        }

        auto* selectionSystem = simulation->GetSelectionSystem();
        auto* commandSystem = simulation->GetCommandSystem();
        if (!selectionSystem || !commandSystem || !selectionSystem->HasSelection()) {
            return;
        }

        uint16_t targetRow = 0;
        uint16_t targetCol = 0;
        if (!tileMap->ScreenToTile(screenX, screenY, targetRow, targetCol, camera)) {
            if (m_logger) {
                m_logger->Warning("Right-click outside map bounds");
            }
            return;
        }

        const Engine::Tile* targetTile = tileMap->GetTile(targetRow, targetCol);
        if (!targetTile || !targetTile->IsWalkable()) {
            if (m_logger) {
                m_logger->Warning("Cannot move to obstacle tile");
            }
            return;
        }

        Engine::TilePosition targetPos(targetRow, targetCol);

        for (auto* character : selectionSystem->GetSelectedCharacters()) {
            if (!character) {
                continue;
            }
            if (targetPos == character->GetTilePosition()) {
                continue;
            }

            World::MoveCommand cmd(targetPos);
            if (m_inputManager->IsShiftHeld()) {
                commandSystem->QueueCommand(character->GetId(), cmd);
            } else {
                commandSystem->IssueCommand(character->GetId(), cmd);
            }
        }
    }

} // namespace LegalCrime
