#include "SelectionSystem.h"
#include "../World.h"
#include "../../Entities/Character.h"
#include "../../../Engine/World/TileMap.h"
#include "../../../Engine/Camera/Camera2D.h"
#include "../../../Engine/Input/InputManager.h"
#include "../../../Engine/Core/Logger/ILogger.h"
#include "../../Input/GameInputBindings.h"
#include <algorithm>
#include <cmath>

using namespace LegalCrime::InputBindings;

namespace LegalCrime {
namespace World {

    SelectionSystem::SelectionSystem(Engine::ILogger* logger)
        : m_logger(logger)
        , m_boxSelecting(false)
        , m_hasHoveredTile(false)
        , m_hoveredTile() {
    }

    SelectionSystem::~SelectionSystem() = default;

    void SelectionSystem::Update(
        World* world,
        Engine::Input::InputManager* inputManager,
        Engine::TileMap* tileMap,
        Engine::Camera2D* camera) {

        if (!world || !inputManager || !tileMap) return;

        // Update hovered tile
        Engine::Input::MousePosition mousePos = inputManager->GetMousePosition();
        uint16_t hoveredRow, hoveredCol;
        m_hasHoveredTile = tileMap->ScreenToTile(mousePos.x, mousePos.y, hoveredRow, hoveredCol, camera);
        if (m_hasHoveredTile) {
            m_hoveredTile = Engine::TilePosition(hoveredRow, hoveredCol);
        }

        // Handle box selection (left mouse drag)
        HandleBoxSelect(world, inputManager, tileMap, camera);

        // Handle control groups (ctrl+0..9)
        HandleControlGroups(inputManager);

        // Handle single click selection (left mouse button, not dragging)
        if (!m_boxSelecting && inputManager->WasActionJustPressed(Actions::SELECT)) {
            uint16_t clickedRow, clickedCol;
            if (tileMap->ScreenToTile(mousePos.x, mousePos.y, clickedRow, clickedCol, camera)) {
                Entities::Character* character = world->GetCharacterAtTile(clickedRow, clickedCol);
                if (character) {
                    if (inputManager->IsShiftHeld()) {
                        // Shift+click: toggle in/out of selection
                        if (IsSelected(character)) {
                            RemoveFromSelection(character);
                        } else {
                            AddToSelection(character);
                        }
                    } else {
                        // Plain click: select only this character
                        SelectCharacter(character);
                    }
                } else if (!inputManager->IsShiftHeld()) {
                    ClearSelection();
                }
            }
        }
    }

    void SelectionSystem::HandleBoxSelect(World* world, Engine::Input::InputManager* input,
                                           Engine::TileMap* tileMap, Engine::Camera2D* camera) {
        Engine::Input::MousePosition mousePos = input->GetMousePosition();

        if (input->IsMouseButtonPressed(Engine::Input::MouseButton::Left)) {
            if (!m_boxSelecting) {
                // Start box selection
                m_boxSelecting = true;
                m_boxStart = Engine::Point(mousePos.x, mousePos.y);
                m_boxEnd = m_boxStart;
            } else {
                m_boxEnd = Engine::Point(mousePos.x, mousePos.y);
            }
        } else if (m_boxSelecting) {
            // Mouse released — finish box selection
            m_boxSelecting = false;

            int dx = std::abs(m_boxEnd.x - m_boxStart.x);
            int dy = std::abs(m_boxEnd.y - m_boxStart.y);

            // Only treat as box select if dragged more than a few pixels
            if (dx > 5 || dy > 5) {
                Engine::Rect boxRect = GetBoxSelectRect();
                std::vector<Entities::Character*> found;

                // Check every character to see if they're within the box
                for (auto* character : world->GetAllCharacters()) {
                    Engine::Point charScreenPos = tileMap->TileToScreenCenter(
                        character->GetTilePosition(), camera);

                    if (charScreenPos.x >= boxRect.x && charScreenPos.x <= boxRect.x + boxRect.w &&
                        charScreenPos.y >= boxRect.y && charScreenPos.y <= boxRect.y + boxRect.h) {
                        found.push_back(character);
                        if (found.size() >= MAX_SELECTION) break;
                    }
                }

                if (!found.empty()) {
                    if (input->IsShiftHeld()) {
                        // Shift+box: add to existing selection
                        for (auto* c : found) {
                            AddToSelection(c);
                        }
                    } else {
                        SetSelection(found);
                    }
                } else if (!input->IsShiftHeld()) {
                    ClearSelection();
                }
            }
        }
    }

    void SelectionSystem::HandleControlGroups(Engine::Input::InputManager* input) {
        // Check ctrl+0..9 for save, plain 0..9 for recall
        for (int i = 0; i < NUM_GROUPS; ++i) {
            SDL_Keycode key = SDLK_0 + i;
            if (input->IsKeyPressed(key)) {
                if (input->IsCtrlHeld()) {
                    SaveGroup(i);
                } else {
                    RecallGroup(i);
                }
            }
        }
    }

    Entities::Character* SelectionSystem::GetSelectedCharacter() {
        return m_selected.empty() ? nullptr : m_selected.front();
    }

    const Entities::Character* SelectionSystem::GetSelectedCharacter() const {
        return m_selected.empty() ? nullptr : m_selected.front();
    }

    void SelectionSystem::SelectCharacter(Entities::Character* character) {
        m_selected.clear();
        if (character) {
            m_selected.push_back(character);
        }
    }

    void SelectionSystem::AddToSelection(Entities::Character* character) {
        if (!character || m_selected.size() >= MAX_SELECTION) return;
        if (!IsSelected(character)) {
            m_selected.push_back(character);
        }
    }

    void SelectionSystem::RemoveFromSelection(Entities::Character* character) {
        m_selected.erase(
            std::remove(m_selected.begin(), m_selected.end(), character),
            m_selected.end());
    }

    void SelectionSystem::SetSelection(const std::vector<Entities::Character*>& characters) {
        m_selected.clear();
        size_t count = std::min(characters.size(), MAX_SELECTION);
        m_selected.assign(characters.begin(), characters.begin() + count);
    }

    void SelectionSystem::ClearSelection() {
        m_selected.clear();
    }

    Engine::Rect SelectionSystem::GetBoxSelectRect() const {
        int x = std::min(m_boxStart.x, m_boxEnd.x);
        int y = std::min(m_boxStart.y, m_boxEnd.y);
        int w = std::abs(m_boxEnd.x - m_boxStart.x);
        int h = std::abs(m_boxEnd.y - m_boxStart.y);
        return Engine::Rect(x, y, w, h);
    }

    void SelectionSystem::SaveGroup(int groupIndex) {
        if (groupIndex < 0 || groupIndex >= NUM_GROUPS) return;
        m_groups[groupIndex] = m_selected;
        if (m_logger) {
            m_logger->Debug("Control group " + std::to_string(groupIndex) +
                          " saved with " + std::to_string(m_selected.size()) + " units");
        }
    }

    void SelectionSystem::RecallGroup(int groupIndex) {
        if (groupIndex < 0 || groupIndex >= NUM_GROUPS) return;
        m_selected = m_groups[groupIndex];
        if (m_logger) {
            m_logger->Debug("Control group " + std::to_string(groupIndex) +
                          " recalled with " + std::to_string(m_selected.size()) + " units");
        }
    }

    bool SelectionSystem::GetHoveredTile(uint16_t& outRow, uint16_t& outCol) const {
        if (m_hasHoveredTile) {
            outRow = m_hoveredTile.row;
            outCol = m_hoveredTile.col;
            return true;
        }
        return false;
    }

    std::optional<Engine::TilePosition> SelectionSystem::GetHoveredTilePosition() const {
        return m_hasHoveredTile ? std::optional(m_hoveredTile) : std::nullopt;
    }

    Entities::Character* SelectionSystem::GetCharacterAtScreenPosition(
        World* world, int screenX, int screenY,
        Engine::TileMap* tileMap, Engine::Camera2D* camera) {
        if (!world || !tileMap) return nullptr;
        uint16_t row, col;
        if (tileMap->ScreenToTile(screenX, screenY, row, col, camera)) {
            return world->GetCharacterAtTile(row, col);
        }
        return nullptr;
    }

    bool SelectionSystem::IsSelected(Entities::Character* character) const {
        return std::find(m_selected.begin(), m_selected.end(), character) != m_selected.end();
    }

} // namespace World
} // namespace LegalCrime
