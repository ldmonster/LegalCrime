#include "MovementSystem.h"
#include "../World.h"
#include "../../Entities/Character.h"
#include "../../../Engine/World/TileMap.h"
#include "../../../Engine/Core/Logger/ILogger.h"

namespace LegalCrime {
namespace World {

    MovementSystem::MovementSystem(Engine::ILogger* logger)
        : m_logger(logger)
        , m_tileMap(nullptr)
        , m_pathfinder(nullptr) {

        if (m_logger) {
            m_logger->Debug("MovementSystem created");
        }
    }

    MovementSystem::~MovementSystem() {
        if (m_logger) {
            m_logger->Debug("MovementSystem destroyed");
        }
    }

    void MovementSystem::Initialize(Engine::TileMap* tileMap) {
        if (!tileMap) {
            if (m_logger) {
                m_logger->Error("MovementSystem::Initialize - TileMap is null");
            }
            return;
        }

        m_tileMap = tileMap;
        m_pathfinder = std::make_unique<Engine::Pathfinding>();

        if (m_logger) {
            m_logger->Info("MovementSystem initialized with tilemap: " +
                         std::to_string(tileMap->GetWidth()) + "x" +
                         std::to_string(tileMap->GetHeight()));
        }
    }

    void MovementSystem::Update(World* world, float deltaTime) {
        if (!world) {
            return;
        }

        // Update all moving characters
        for (auto& state : m_movingCharacters) {
            if (state.isMoving && state.character) {
                UpdateCharacterMovement(state, deltaTime);
            }
        }

        // Remove completed movements
        m_movingCharacters.erase(
            std::remove_if(m_movingCharacters.begin(), m_movingCharacters.end(),
                [](const MovementState& state) { return !state.isMoving; }),
            m_movingCharacters.end()
        );
    }

    bool MovementSystem::MoveCharacterToTile(
        Entities::Character* character,
        uint16_t targetRow,
        uint16_t targetCol,
        float duration) {

        if (!character || !m_pathfinder || !m_tileMap) {
            if (m_logger) {
                m_logger->Warning("MovementSystem::MoveCharacterToTile - Invalid character or pathfinder not initialized");
            }
            return false;
        }

        uint16_t currentRow = character->GetTileRow();
        uint16_t currentCol = character->GetTileCol();

        // Create walkability callback that queries the tilemap
        auto isWalkable = [this](uint16_t row, uint16_t col) -> bool {
            const Engine::Tile* tile = m_tileMap->GetTile(row, col);
            return tile && tile->IsWalkable();
        };

        // Find path using correct API
        Engine::Path path = m_pathfinder->FindPath(
            Engine::TilePosition(currentRow, currentCol),
            Engine::TilePosition(targetRow, targetCol),
            m_tileMap->GetWidth(),
            m_tileMap->GetHeight(),
            isWalkable
        );

        if (path.empty()) {
            if (m_logger) {
                m_logger->Warning("MovementSystem::MoveCharacterToTile - No path found from (" +
                                std::to_string(currentRow) + "," + std::to_string(currentCol) + ") to (" +
                                std::to_string(targetRow) + "," + std::to_string(targetCol) + ")");
            }
            return false;
        }

        return MoveCharacterAlongPath(character, path, duration);
    }

    bool MovementSystem::MoveCharacterAlongPath(
        Entities::Character* character,
        const Engine::Path& path,
        float moveDuration) {
        
        if (!character || path.empty()) {
            return false;
        }

        MovementState* state = GetOrCreateMovementState(character);
        if (!state) {
            return false;
        }

        state->character = character;
        state->currentPath = path;
        state->currentPathIndex = 0;
        state->moveDuration = moveDuration;
        state->moveTime = 0.0f;
        state->isMoving = true;

        // Start movement to first tile in path
        if (!path.empty()) {
            const Engine::TilePosition& firstTile = path[0];
            state->targetRow = firstTile.row;
            state->targetCol = firstTile.col;

            // Update animation based on direction
            UpdateCharacterAnimation(
                character,
                character->GetTileRow(),
                character->GetTileCol(),
                firstTile.row,
                firstTile.col
            );
        }

        if (m_logger) {
            m_logger->Debug("Character movement started along path with " +
                          std::to_string(path.size()) + " tiles");
        }

        return true;
    }

    void MovementSystem::StopCharacterMovement(Entities::Character* character) {
        if (!character) {
            return;
        }

        MovementState* state = GetMovementState(character);
        if (state) {
            state->isMoving = false;
            state->currentPath.clear();
            character->SetAnimation("idle_down");  // Default idle animation

            if (m_logger) {
                m_logger->Debug("Character movement stopped");
            }
        }
    }

    bool MovementSystem::IsCharacterMoving(const Entities::Character* character) const {
        if (!character) {
            return false;
        }

        const MovementState* state = GetMovementState(character);
        return state && state->isMoving;
    }

    MovementSystem::MovementState* MovementSystem::GetOrCreateMovementState(Entities::Character* character) {
        if (!character) {
            return nullptr;
        }

        // Check if state already exists
        for (auto& state : m_movingCharacters) {
            if (state.character == character) {
                return &state;
            }
        }

        // Create new state
        MovementState newState;
        newState.character = character;
        m_movingCharacters.push_back(newState);
        return &m_movingCharacters.back();
    }

    MovementSystem::MovementState* MovementSystem::GetMovementState(const Entities::Character* character) {
        for (auto& state : m_movingCharacters) {
            if (state.character == character) {
                return &state;
            }
        }
        return nullptr;
    }

    const MovementSystem::MovementState* MovementSystem::GetMovementState(const Entities::Character* character) const {
        for (const auto& state : m_movingCharacters) {
            if (state.character == character) {
                return &state;
            }
        }
        return nullptr;
    }

    void MovementSystem::UpdateCharacterMovement(MovementState& state, float deltaTime) {
        if (!state.character || !state.isMoving) {
            return;
        }

        state.moveTime += deltaTime;

        if (state.moveTime >= state.moveDuration) {
            // Movement to current tile complete
            state.moveTime = 0.0f;
            state.character->SetTilePosition(state.targetRow, state.targetCol);

            // Check if there's more path to follow
            if (!state.currentPath.empty() && state.currentPathIndex < state.currentPath.size() - 1) {
                // Move to next tile in path
                state.currentPathIndex++;
                const Engine::TilePosition& nextTile = state.currentPath[state.currentPathIndex];

                uint16_t currentRow = state.character->GetTileRow();
                uint16_t currentCol = state.character->GetTileCol();

                state.targetRow = nextTile.row;
                state.targetCol = nextTile.col;

                // Update animation for new direction
                UpdateCharacterAnimation(state.character, currentRow, currentCol, nextTile.row, nextTile.col);
            } else {
                // Path complete
                state.isMoving = false;
                state.currentPath.clear();
                state.character->SetAnimation("idle_down");  // Set to idle

                if (m_logger) {
                    m_logger->Debug("Character reached destination");
                }
            }
        }
    }

    void MovementSystem::UpdateCharacterAnimation(
        Entities::Character* character,
        uint16_t fromRow,
        uint16_t fromCol,
        uint16_t toRow,
        uint16_t toCol) {
        
        if (!character) {
            return;
        }

        // Determine direction based on tile delta
        int deltaRow = static_cast<int>(toRow) - static_cast<int>(fromRow);
        int deltaCol = static_cast<int>(toCol) - static_cast<int>(fromCol);

        if (deltaRow < 0) {
            character->SetAnimation("walk_up");
        } else if (deltaRow > 0) {
            character->SetAnimation("walk_down");
        } else if (deltaCol < 0) {
            character->SetAnimation("walk_left");
        } else if (deltaCol > 0) {
            character->SetAnimation("walk_right");
        }
    }

} // namespace World
} // namespace LegalCrime
