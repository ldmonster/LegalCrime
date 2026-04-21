#include "SelectionSystem.h"
#include "../World.h"
#include "../../Entities/Character.h"
#include "../../../Engine/World/TileMap.h"
#include "../../../Engine/Camera/Camera2D.h"
#include "../../../Engine/Core/Logger/ILogger.h"
#include <algorithm>
#include <cmath>

namespace LegalCrime {
namespace World {

    SelectionSystem::SelectionSystem(Engine::ILogger* logger)
        : m_logger(logger)
        , m_boxSelecting(false)
        , m_hasHoveredTile(false)
        , m_hoveredTile() {
    }

    SelectionSystem::~SelectionSystem() = default;

    void SelectionSystem::UpdateHoveredTile(int screenX, int screenY, Engine::TileMap* tileMap, Engine::Camera2D* camera) {
        if (!tileMap) {
            m_hasHoveredTile = false;
            return;
        }

        uint16_t hoveredRow = 0;
        uint16_t hoveredCol = 0;
        m_hasHoveredTile = tileMap->ScreenToTile(screenX, screenY, hoveredRow, hoveredCol, camera);
        if (m_hasHoveredTile) {
            m_hoveredTile = Engine::TilePosition(hoveredRow, hoveredCol);
        }
    }

    void SelectionSystem::StartBoxSelection(const Engine::Point& startScreenPos) {
        m_boxSelecting = true;
        m_boxStart = startScreenPos;
        m_boxEnd = startScreenPos;
    }

    void SelectionSystem::UpdateBoxSelection(const Engine::Point& currentScreenPos) {
        if (!m_boxSelecting) {
            return;
        }
        m_boxEnd = currentScreenPos;
    }

    void SelectionSystem::EndBoxSelection(
        World* world,
        Engine::TileMap* tileMap,
        Engine::Camera2D* camera,
        bool appendToExisting) {

        if (!m_boxSelecting) {
            return;
        }

        m_boxSelecting = false;

        if (!world || !tileMap) {
            return;
        }

        int dx = std::abs(m_boxEnd.x - m_boxStart.x);
        int dy = std::abs(m_boxEnd.y - m_boxStart.y);
        if (dx <= 5 && dy <= 5) {
            return;
        }

        Engine::Rect boxRect = GetBoxSelectRect();
        std::vector<Entities::Character*> found;

        // Spatial grid narrows candidate set for large worlds.
        std::vector<Engine::Entity*> candidates = world->GetEntitiesInRect(boxRect);
        for (auto* entity : candidates) {
            auto* character = dynamic_cast<Entities::Character*>(entity);
            if (!character) {
                continue;
            }

            Engine::Point charScreenPos = tileMap->TileToScreenCenter(character->GetTilePosition(), camera);
            if (charScreenPos.x >= boxRect.x && charScreenPos.x <= boxRect.x + boxRect.w &&
                charScreenPos.y >= boxRect.y && charScreenPos.y <= boxRect.y + boxRect.h) {
                found.push_back(character);
                if (found.size() >= MAX_SELECTION) {
                    break;
                }
            }
        }

        if (!found.empty()) {
            if (appendToExisting) {
                for (auto* c : found) {
                    AddToSelection(c);
                }
            } else {
                SetSelection(found);
            }
        } else if (!appendToExisting) {
            ClearSelection();
        }
    }

    void SelectionSystem::SelectAtScreenPosition(
        World* world,
        int screenX,
        int screenY,
        Engine::TileMap* tileMap,
        Engine::Camera2D* camera,
        bool toggleWithShift,
        bool clearIfNoCharacter) {

        if (!world || !tileMap) {
            return;
        }

        uint16_t clickedRow = 0;
        uint16_t clickedCol = 0;
        if (!tileMap->ScreenToTile(screenX, screenY, clickedRow, clickedCol, camera)) {
            return;
        }

        Entities::Character* character = world->GetCharacterAtTile(clickedRow, clickedCol);
        if (character) {
            if (toggleWithShift) {
                if (IsSelected(character)) {
                    RemoveFromSelection(character);
                } else {
                    AddToSelection(character);
                }
            } else {
                SelectCharacter(character);
            }
        } else if (clearIfNoCharacter) {
            ClearSelection();
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

    void SelectionSystem::PruneSelectionToWorld(const World* world) {
        if (!world) {
            m_selected.clear();
            for (auto& group : m_groups) {
                group.clear();
            }
            return;
        }

        const auto& aliveCharacters = world->GetAllCharacters();
        auto isAliveCharacter = [&](Entities::Character* character) {
            return character &&
                std::find(aliveCharacters.begin(), aliveCharacters.end(), character) != aliveCharacters.end();
        };

        m_selected.erase(
            std::remove_if(m_selected.begin(), m_selected.end(),
                [&](Entities::Character* character) { return !isAliveCharacter(character); }),
            m_selected.end());

        for (auto& group : m_groups) {
            group.erase(
                std::remove_if(group.begin(), group.end(),
                    [&](Entities::Character* character) { return !isAliveCharacter(character); }),
                group.end());
        }
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
