#include "SelectionSystem.h"
#include "../World.h"
#include "../../Entities/Character.h"
#include "../../../Engine/World/TileMap.h"
#include "../../../Engine/Camera/Camera2D.h"
#include "../../../Engine/Input/InputManager.h"
#include "../../../Engine/Core/Logger/ILogger.h"
#include "../../Input/GameInputBindings.h"

using namespace LegalCrime::InputBindings;

namespace LegalCrime {
namespace World {

    SelectionSystem::SelectionSystem(Engine::ILogger* logger)
        : m_logger(logger)
        , m_selectedCharacter(nullptr)
        , m_hasHoveredTile(false)
        , m_hoveredRow(0)
        , m_hoveredCol(0) {
        
        if (m_logger) {
            m_logger->Debug("SelectionSystem created");
        }
    }

    SelectionSystem::~SelectionSystem() {
        if (m_logger) {
            m_logger->Debug("SelectionSystem destroyed");
        }
    }

    void SelectionSystem::Update(
        World* world,
        Engine::Input::InputManager* inputManager,
        Engine::TileMap* tileMap,
        Engine::Camera2D* camera) {
        
        if (!world || !inputManager || !tileMap) {
            return;
        }

        // Update hovered tile for UI feedback
        Engine::Input::MousePosition mousePos = inputManager->GetMousePosition();
        uint16_t hoveredRow, hoveredCol;
        m_hasHoveredTile = tileMap->ScreenToTile(
            mousePos.x,
            mousePos.y,
            hoveredRow,
            hoveredCol,
            camera
        );

        if (m_hasHoveredTile) {
            m_hoveredRow = hoveredRow;
            m_hoveredCol = hoveredCol;
        }

        // Handle selection (left mouse button)
        if (inputManager->WasActionJustPressed(Actions::SELECT)) {
            uint16_t clickedRow, clickedCol;
            if (tileMap->ScreenToTile(mousePos.x, mousePos.y, clickedRow, clickedCol, camera)) {
                // Check if there's a character at this tile
                Entities::Character* character = world->GetCharacterAtTile(clickedRow, clickedCol);

                if (character) {
                    SelectCharacter(character);
                } else {
                    // Clicked on empty tile - deselect
                    ClearSelection();
                }
            }
        }
    }

    void SelectionSystem::SelectCharacter(Entities::Character* character) {
        if (m_selectedCharacter == character) {
            // Already selected
            return;
        }

        m_selectedCharacter = character;

        if (m_logger && character) {
            m_logger->Debug("Character selected at tile (" +
                          std::to_string(character->GetTileRow()) + ", " +
                          std::to_string(character->GetTileCol()) + ")");
        }
    }

    void SelectionSystem::ClearSelection() {
        if (m_selectedCharacter) {
            m_selectedCharacter = nullptr;

            if (m_logger) {
                m_logger->Debug("Selection cleared");
            }
        }
    }

    bool SelectionSystem::GetHoveredTile(uint16_t& outRow, uint16_t& outCol) const {
        if (m_hasHoveredTile) {
            outRow = m_hoveredRow;
            outCol = m_hoveredCol;
            return true;
        }
        return false;
    }

    Entities::Character* SelectionSystem::GetCharacterAtScreenPosition(
        World* world,
        int screenX,
        int screenY,
        Engine::TileMap* tileMap,
        Engine::Camera2D* camera) {
        
        if (!world || !tileMap) {
            return nullptr;
        }

        uint16_t row, col;
        if (tileMap->ScreenToTile(screenX, screenY, row, col, camera)) {
            return world->GetCharacterAtTile(row, col);
        }

        return nullptr;
    }

} // namespace World
} // namespace LegalCrime
