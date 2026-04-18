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
        std::vector<uint32_t> completed;
        for (auto& [id, state] : m_movingCharacters) {
            if (state.isMoving && state.character) {
                UpdateCharacterMovement(state, deltaTime);
            }
            if (!state.isMoving) {
                completed.push_back(id);
            }
        }

        // Remove completed movements
        for (uint32_t id : completed) {
            m_movingCharacters.erase(id);
        }
    }

    bool MovementSystem::MoveCharacterToTile(
        Entities::Character* character,
        const Engine::TilePosition& target,
        float duration) {

        if (!character || !m_pathfinder || !m_tileMap) {
            if (m_logger) {
                m_logger->Warning("MovementSystem::MoveCharacterToTile - Invalid character or pathfinder not initialized");
            }
            return false;
        }

        Engine::TilePosition current = character->GetTilePosition();

        // Create walkability callback that queries the tilemap
        auto isWalkable = [this](const Engine::TilePosition& pos) -> bool {
            const Engine::Tile* tile = m_tileMap->GetTile(pos.row, pos.col);
            return tile && tile->IsWalkable();
        };

        // Find path using correct API
        Engine::Path path = m_pathfinder->FindPath(
            current,
            target,
            m_tileMap->GetWidth(),
            m_tileMap->GetHeight(),
            isWalkable
        );

        if (path.empty()) {
            if (m_logger) {
                m_logger->Warning("MovementSystem::MoveCharacterToTile - No path found from (" +
                                std::to_string(current.row) + "," + std::to_string(current.col) + ") to (" +
                                std::to_string(target.row) + "," + std::to_string(target.col) + ")");
            }
            return false;
        }

        return MoveCharacterAlongPath(character, path, duration);
    }

    bool MovementSystem::MoveCharacterToTile(
        Entities::Character* character,
        uint16_t targetRow,
        uint16_t targetCol,
        float duration) {
        return MoveCharacterToTile(character, Engine::TilePosition(targetRow, targetCol), duration);
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
            state->target = firstTile;

            // Update animation based on direction
            Engine::TilePosition from = character->GetTilePosition();
            UpdateCharacterAnimation(character, from.row, from.col, firstTile.row, firstTile.col);
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

        uint32_t id = character->GetId();
        auto it = m_movingCharacters.find(id);
        if (it != m_movingCharacters.end()) {
            return &it->second;
        }

        // Create new state
        MovementState& newState = m_movingCharacters[id];
        newState.character = character;
        return &newState;
    }

    MovementSystem::MovementState* MovementSystem::GetMovementState(const Entities::Character* character) {
        if (!character) return nullptr;
        auto it = m_movingCharacters.find(character->GetId());
        return (it != m_movingCharacters.end()) ? &it->second : nullptr;
    }

    const MovementSystem::MovementState* MovementSystem::GetMovementState(const Entities::Character* character) const {
        if (!character) return nullptr;
        auto it = m_movingCharacters.find(character->GetId());
        return (it != m_movingCharacters.end()) ? &it->second : nullptr;
    }

    void MovementSystem::UpdateCharacterMovement(MovementState& state, float deltaTime) {
        if (!state.character || !state.isMoving) {
            return;
        }

        state.moveTime += deltaTime;

        if (state.moveTime >= state.moveDuration) {
            // Movement to current tile complete
            state.moveTime = 0.0f;
            state.character->SetTilePosition(state.target);

            // Check if there's more path to follow
            if (!state.currentPath.empty() && state.currentPathIndex < state.currentPath.size() - 1) {
                // Move to next tile in path
                state.currentPathIndex++;
                const Engine::TilePosition& nextTile = state.currentPath[state.currentPathIndex];

                Engine::TilePosition current = state.character->GetTilePosition();
                state.target = nextTile;

                // Update animation for new direction
                UpdateCharacterAnimation(state.character, current.row, current.col, nextTile.row, nextTile.col);
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

        int deltaRow = static_cast<int>(toRow) - static_cast<int>(fromRow);
        int deltaCol = static_cast<int>(toCol) - static_cast<int>(fromCol);
        Engine::Direction dir = Engine::DirectionUtil::FromDelta(deltaRow, deltaCol);
        character->SetAnimation(Engine::DirectionUtil::ToAnimationName(dir));
    }

} // namespace World
} // namespace LegalCrime
