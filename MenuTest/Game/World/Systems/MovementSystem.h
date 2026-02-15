#pragma once

#include "../../../Engine/Core/Types.h"
#include "../../../Engine/World/Pathfinding.h"
#include <memory>

namespace Engine {
    class ILogger;
    class TileMap;
}

namespace LegalCrime {
namespace Entities {
    class Character;
}
namespace World {
    class World;

    /// <summary>
    /// MovementSystem handles character movement, pathfinding, and tile-based navigation.
    /// Manages movement state, path following, and animation updates for moving characters.
    /// </summary>
    class MovementSystem {
    public:
        MovementSystem(Engine::ILogger* logger = nullptr);
        ~MovementSystem();

        // Initialize with tilemap (for pathfinding)
        void Initialize(Engine::TileMap* tileMap);

        // Update all moving characters
        void Update(World* world, float deltaTime);

        // Request character movement to a tile
        bool MoveCharacterToTile(
            Entities::Character* character,
            uint16_t targetRow,
            uint16_t targetCol,
            float duration = 0.3f
        );

        // Request character movement along a path
        bool MoveCharacterAlongPath(
            Entities::Character* character,
            const Engine::Path& path,
            float moveDuration = 0.3f
        );

        // Stop character movement immediately
        void StopCharacterMovement(Entities::Character* character);

        // Check if character is currently moving
        bool IsCharacterMoving(const Entities::Character* character) const;

        // Get pathfinder (for external path queries)
        Engine::Pathfinding* GetPathfinder() { return m_pathfinder.get(); }

    private:
        struct MovementState {
            Entities::Character* character;
            uint16_t targetRow;
            uint16_t targetCol;
            Engine::Path currentPath;
            size_t currentPathIndex;
            float moveTime;
            float moveDuration;
            bool isMoving;

            MovementState()
                : character(nullptr)
                , targetRow(0)
                , targetCol(0)
                , currentPathIndex(0)
                , moveTime(0.0f)
                , moveDuration(0.3f)
                , isMoving(false) {}
        };

        Engine::ILogger* m_logger;
        Engine::TileMap* m_tileMap;
        std::unique_ptr<Engine::Pathfinding> m_pathfinder;
        std::vector<MovementState> m_movingCharacters;

        // Internal methods
        MovementState* GetOrCreateMovementState(Entities::Character* character);
        MovementState* GetMovementState(const Entities::Character* character);
        const MovementState* GetMovementState(const Entities::Character* character) const;
        void UpdateCharacterMovement(MovementState& state, float deltaTime);
        void UpdateCharacterAnimation(Entities::Character* character, uint16_t fromRow, uint16_t fromCol, uint16_t toRow, uint16_t toCol);
    };

} // namespace World
} // namespace LegalCrime
