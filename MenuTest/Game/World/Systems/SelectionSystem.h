#pragma once

#include "../../../Engine/Core/Types.h"
#include <memory>

namespace Engine {
    class ILogger;
    class TileMap;
    class Camera2D;
    namespace Input {
        class InputManager;
    }
}

namespace LegalCrime {
namespace Entities {
    class Character;
}
namespace World {
    class World;

    /// <summary>
    /// SelectionSystem handles entity selection through mouse input.
    /// Manages selection state, highlighting, and selection queries.
    /// </summary>
    class SelectionSystem {
    public:
        SelectionSystem(Engine::ILogger* logger = nullptr);
        ~SelectionSystem();

        // Update selection (process input)
        void Update(
            World* world,
            Engine::Input::InputManager* inputManager,
            Engine::TileMap* tileMap,
            Engine::Camera2D* camera
        );

        // Selection queries
        Entities::Character* GetSelectedCharacter() { return m_selectedCharacter; }
        const Entities::Character* GetSelectedCharacter() const { return m_selectedCharacter; }
        bool HasSelection() const { return m_selectedCharacter != nullptr; }

        // Selection control
        void SelectCharacter(Entities::Character* character);
        void ClearSelection();

        // Get the currently hovered tile (for UI feedback)
        bool GetHoveredTile(uint16_t& outRow, uint16_t& outCol) const;

        // Utility - get character at screen position
        Entities::Character* GetCharacterAtScreenPosition(
            World* world,
            int screenX,
            int screenY,
            Engine::TileMap* tileMap,
            Engine::Camera2D* camera
        );

    private:
        Engine::ILogger* m_logger;
        Entities::Character* m_selectedCharacter;

        // Hover state (for UI feedback)
        bool m_hasHoveredTile;
        uint16_t m_hoveredRow;
        uint16_t m_hoveredCol;
    };

} // namespace World
} // namespace LegalCrime
