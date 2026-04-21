#pragma once

#include "../../../Engine/Core/Types.h"
#include <array>
#include <optional>
#include <vector>

namespace Engine {
    class ILogger;
    class TileMap;
    class Camera2D;
}

namespace LegalCrime {
namespace Entities {
    class Character;
}
namespace World {
    class World;

    /// Multi-unit selection system with box select, shift-click, and control groups.
    /// Supports selection of up to MAX_SELECTION units.
    class SelectionSystem {
    public:
        static constexpr size_t MAX_SELECTION = 200;
        static constexpr int NUM_GROUPS = 10; // ctrl+0..9

        SelectionSystem(Engine::ILogger* logger = nullptr);
        ~SelectionSystem();

        // Hover and box selection primitives (input-agnostic).
        void UpdateHoveredTile(int screenX, int screenY, Engine::TileMap* tileMap, Engine::Camera2D* camera);
        void StartBoxSelection(const Engine::Point& startScreenPos);
        void UpdateBoxSelection(const Engine::Point& currentScreenPos);
        void EndBoxSelection(World* world, Engine::TileMap* tileMap, Engine::Camera2D* camera, bool appendToExisting);

        // Click selection primitive (input-agnostic).
        void SelectAtScreenPosition(
            World* world,
            int screenX,
            int screenY,
            Engine::TileMap* tileMap,
            Engine::Camera2D* camera,
            bool toggleWithShift,
            bool clearIfNoCharacter
        );

        // --- Multi-selection queries ---
        const std::vector<Entities::Character*>& GetSelectedCharacters() const { return m_selected; }
        bool HasSelection() const { return !m_selected.empty(); }
        size_t GetSelectionCount() const { return m_selected.size(); }

        // Legacy single-selection convenience (returns first selected or nullptr)
        Entities::Character* GetSelectedCharacter();
        const Entities::Character* GetSelectedCharacter() const;

        // --- Selection control ---
        void SelectCharacter(Entities::Character* character);
        void AddToSelection(Entities::Character* character);
        void RemoveFromSelection(Entities::Character* character);
        void SetSelection(const std::vector<Entities::Character*>& characters);
        void ClearSelection();
        void PruneSelectionToWorld(const World* world);

        // --- Box selection ---
        bool IsBoxSelecting() const { return m_boxSelecting; }
        Engine::Rect GetBoxSelectRect() const;

        // --- Control groups (ctrl+0..9) ---
        void SaveGroup(int groupIndex);
        void RecallGroup(int groupIndex);

        // --- Hover ---
        bool GetHoveredTile(uint16_t& outRow, uint16_t& outCol) const;
        std::optional<Engine::TilePosition> GetHoveredTilePosition() const;

        // Utility
        Entities::Character* GetCharacterAtScreenPosition(
            World* world,
            int screenX,
            int screenY,
            Engine::TileMap* tileMap,
            Engine::Camera2D* camera
        );

    private:
        bool IsSelected(Entities::Character* character) const;

        Engine::ILogger* m_logger;

        // Multi-selection
        std::vector<Entities::Character*> m_selected;

        // Box selection state
        bool m_boxSelecting;
        Engine::Point m_boxStart;
        Engine::Point m_boxEnd;

        // Control groups
        std::array<std::vector<Entities::Character*>, NUM_GROUPS> m_groups;

        // Hover state
        bool m_hasHoveredTile;
        Engine::TilePosition m_hoveredTile;
    };

} // namespace World
} // namespace LegalCrime
