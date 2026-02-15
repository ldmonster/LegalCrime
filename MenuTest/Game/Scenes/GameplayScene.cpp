#include "GameplayScene.h"
#include "../../Engine/World/TileMap.h"
#include "../../Engine/Graphics/AnimatedSprite.h"

namespace LegalCrime {

    // Private implementation to hide old game code dependencies
    class GameplaySceneImpl {
    public:
        std::unique_ptr<Engine::TileMap> tileMap;
        std::unique_ptr<Engine::AnimatedSprite> character;

        // Character tile position
        uint16_t characterRow;
        uint16_t characterCol;

        // Target tile position during movement
        uint16_t targetRow;
        uint16_t targetCol;

        // Movement state
        bool isMoving;
        float moveTime;
        float moveDuration;

        // Direction tracking
        enum class Direction {
            None,
            Up,
            Down,
            Left,
            Right
        };
        Direction currentDirection;

        GameplaySceneImpl()
            : tileMap(nullptr)
            , character(nullptr)
            , characterRow(0)
            , characterCol(0)
            , targetRow(0)
            , targetCol(0)
            , isMoving(false)
            , moveTime(0.0f)
            , moveDuration(0.3f)
            , currentDirection(Direction::None) {
        }

        ~GameplaySceneImpl() {
            Cleanup();
        }

        void Cleanup() {
            character.reset();
            tileMap.reset();
        }
    };

    GameplayScene::GameplayScene(Engine::ILogger* logger, Engine::IRenderer* renderer)
        : Scene("Gameplay", logger, renderer)
        , m_impl(std::make_unique<GameplaySceneImpl>()) {
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

        // Create and initialize new TileMap system (100x100 tiles)
        m_impl->tileMap = std::make_unique<Engine::TileMap>(20, 20, m_logger);

        // Initialize with default window size (800x600)
        // TODO: Get actual window dimensions from window manager/config
        auto initResult = m_impl->tileMap->Initialize(800, 600);
        if (!initResult) {
            return Engine::Result<void>::Failure("Failed to initialize TileMap: " + initResult.error);
        }

        // Load character sprite sheet
        auto characterTexture = Engine::Texture::LoadFromFile(
            m_renderer,
            "Pics/Thug_Frames.png",
            m_logger
        );

        if (characterTexture) {
            m_impl->character = std::make_unique<Engine::AnimatedSprite>(characterTexture, m_logger);

            // Create animations for different directions
            // Note: Adjust rows/columns when you replace the sprite sheet
            // Current setup assumes: each row = different direction, each having multiple frames

            // Idle animation (row 0, frames 0-1)
            auto idleAnim = Engine::CreateGridAnimation(
                "idle",      // Animation name
                50,          // Frame width
                50,          // Frame height
                6,           // Columns
                1,           // Total rows in sheet
                0,           // Start frame
                2,           // Frame count (using first 2 frames for idle)
                0.2f,        // Frame duration (slower for idle)
                true         // Loop
            );

            // Walk down animation (using frames 0-2 for now)
            auto walkDownAnim = Engine::CreateGridAnimation(
                "walk_down",
                50,
                50,
                6,
                1,
                0,           // Start frame 0
                3,           // 3 frames
                0.1f,
                true
            );

            // Walk left animation (using frames 2-4 for now)
            auto walkLeftAnim = Engine::CreateGridAnimation(
                "walk_left",
                50,
                50,
                6,
                1,
                2,           // Start frame 2
                2,           // 2 frames
                0.1f,
                true
            );

            // Walk right animation (using frames 4-5 for now)
            auto walkRightAnim = Engine::CreateGridAnimation(
                "walk_right",
                50,
                50,
                6,
                1,
                4,           // Start frame 4
                2,           // 2 frames
                0.1f,
                true
            );

            // Walk up animation (using frames 0-2, will be same as down for now)
            auto walkUpAnim = Engine::CreateGridAnimation(
                "walk_up",
                50,
                50,
                6,
                1,
                0,           // Start frame 0
                3,           // 3 frames
                0.1f,
                true
            );

            // Add all animations
            m_impl->character->AddAnimation(idleAnim);
            m_impl->character->AddAnimation(walkDownAnim);
            m_impl->character->AddAnimation(walkLeftAnim);
            m_impl->character->AddAnimation(walkRightAnim);
            m_impl->character->AddAnimation(walkUpAnim);

            // Set initial animation to idle
            m_impl->character->SetAnimation("idle");
            m_impl->character->SetScale(2.0f); // Scale up for visibility

            // Place character at center of map
            m_impl->characterRow = m_impl->tileMap->GetHeight() / 2;
            m_impl->characterCol = m_impl->tileMap->GetWidth() / 2;

            // Convert tile position to screen coordinates
            Engine::Point screenPos = m_impl->tileMap->TileToScreenCenter(
                m_impl->characterRow, 
                m_impl->characterCol
            );

            // Offset character position to appear centered on tile
            // Subtract half the sprite size to center it
            int spriteWidth = static_cast<int>(50 * m_impl->character->GetScale());
            int spriteHeight = static_cast<int>(50 * m_impl->character->GetScale());
            screenPos.x -= spriteWidth / 2;
            screenPos.y -= spriteHeight / 2;

            m_impl->character->SetPosition(screenPos.x, screenPos.y);

            if (m_logger) {
                m_logger->Info("Character sprite loaded successfully");
                m_logger->Info("Character placed at tile (" + std::to_string(m_impl->characterRow) + 
                             ", " + std::to_string(m_impl->characterCol) + ") -> screen (" + 
                             std::to_string(screenPos.x) + ", " + std::to_string(screenPos.y) + ")");
            }
        } else {
            if (m_logger) {
                m_logger->Warning("Failed to load character sprite from Pics/Thug_Frames.png");
            }
        }

        m_initialized = true;

        if (m_logger) {
            m_logger->Info("Gameplay Scene initialized successfully with TileMap");
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

        m_impl->Cleanup();
        m_initialized = false;
    }

    void GameplayScene::HandleEvent(const SDL_Event& event) {
        if (!m_initialized || !m_impl->tileMap) {
            return;
        }

        // Handle character movement with WASD or arrow keys
        if (event.type == SDL_EVENT_KEY_DOWN && m_impl->character) {
            // Only allow movement if character is not currently moving
            if (!m_impl->isMoving) {
                uint16_t newRow = m_impl->characterRow;
                uint16_t newCol = m_impl->characterCol;
                bool moved = false;
                GameplaySceneImpl::Direction direction = GameplaySceneImpl::Direction::None;

                switch (event.key.key) {
                    case SDLK_W:
                    case SDLK_UP:
                        // Move north (up in isometric)
                        if (m_impl->characterRow > 0) {
                            newRow--;
                            moved = true;
                            direction = GameplaySceneImpl::Direction::Up;
                        }
                        break;
                    case SDLK_S:
                    case SDLK_DOWN:
                        // Move south (down in isometric)
                        if (m_impl->characterRow < m_impl->tileMap->GetHeight() - 1) {
                            newRow++;
                            moved = true;
                            direction = GameplaySceneImpl::Direction::Down;
                        }
                        break;
                    case SDLK_A:
                    case SDLK_LEFT:
                        // Move west (left in isometric)
                        if (m_impl->characterCol > 0) {
                            newCol--;
                            moved = true;
                            direction = GameplaySceneImpl::Direction::Left;
                        }
                        break;
                    case SDLK_D:
                    case SDLK_RIGHT:
                        // Move east (right in isometric)
                        if (m_impl->characterCol < m_impl->tileMap->GetWidth() - 1) {
                            newCol++;
                            moved = true;
                            direction = GameplaySceneImpl::Direction::Right;
                        }
                        break;
                }

                if (moved) {
                    m_impl->currentDirection = direction;

                    // Change animation based on direction
                    switch (direction) {
                        case GameplaySceneImpl::Direction::Up:
                            m_impl->character->SetAnimation("walk_up");
                            break;
                        case GameplaySceneImpl::Direction::Down:
                            m_impl->character->SetAnimation("walk_down");
                            break;
                        case GameplaySceneImpl::Direction::Left:
                            m_impl->character->SetAnimation("walk_left");
                            break;
                        case GameplaySceneImpl::Direction::Right:
                            m_impl->character->SetAnimation("walk_right");
                            break;
                        default:
                            break;
                    }

                    MoveCharacterToTile(newRow, newCol, 0.3f);
                }
            }
        }

        // Forward events to TileMap for camera panning (only if not a movement key)
        m_impl->tileMap->HandleEvent(event);
    }

    void GameplayScene::Update(float deltaTime) {
        if (!m_initialized || !m_impl->tileMap) {
            return;
        }

        // Update animated character
        if (m_impl->character) {
            // Update animation frames
            m_impl->character->Update(deltaTime);

            // Update movement if active
            if (m_impl->isMoving) {
                m_impl->moveTime += deltaTime;

                if (m_impl->moveTime >= m_impl->moveDuration) {
                    // Movement complete
                    m_impl->isMoving = false;
                    m_impl->moveTime = 0.0f;
                    m_impl->characterRow = m_impl->targetRow;
                    m_impl->characterCol = m_impl->targetCol;

                    // Switch to idle animation when movement stops
                    m_impl->character->SetAnimation("idle");
                    m_impl->currentDirection = GameplaySceneImpl::Direction::None;
                }
            }

            // Always update character screen position based on tile position
            // This ensures character stays centered on tiles even during camera panning
            if (m_impl->isMoving) {
                // During movement, interpolate between tiles
                float progress = m_impl->moveTime / m_impl->moveDuration;

                // Get current tile center
                Engine::Point startPos = m_impl->tileMap->TileToScreenCenter(
                    m_impl->characterRow,
                    m_impl->characterCol
                );

                // Get target tile center
                Engine::Point endPos = m_impl->tileMap->TileToScreenCenter(
                    m_impl->targetRow,
                    m_impl->targetCol
                );

                // Center sprite on tile
                int spriteWidth = static_cast<int>(50 * m_impl->character->GetScale());
                int spriteHeight = static_cast<int>(50 * m_impl->character->GetScale());
                startPos.x -= spriteWidth / 2;
                startPos.y -= spriteHeight / 2;
                endPos.x -= spriteWidth / 2;
                endPos.y -= spriteHeight / 2;

                // Offset character higher on the tile
                int tileHeightOffset = m_impl->tileMap->GetTileHeight();
                startPos.y -= tileHeightOffset;
                endPos.y -= tileHeightOffset;

                // Manually interpolate position based on movement progress
                // This keeps the character moving between tile centers even when camera pans
                int currentX = static_cast<int>(startPos.x + (endPos.x - startPos.x) * progress);
                int currentY = static_cast<int>(startPos.y + (endPos.y - startPos.y) * progress);

                m_impl->character->SetPosition(currentX, currentY);
            } else {
                // Not moving - just center on current tile
                Engine::Point screenPos = m_impl->tileMap->TileToScreenCenter(
                    m_impl->characterRow,
                    m_impl->characterCol
                );

                // Center sprite on tile
                int spriteWidth = static_cast<int>(50 * m_impl->character->GetScale());
                int spriteHeight = static_cast<int>(50 * m_impl->character->GetScale());
                screenPos.x -= spriteWidth / 2;
                screenPos.y -= spriteHeight / 2;

                // Offset character higher on the tile
                int tileHeightOffset = m_impl->tileMap->GetTileHeight();
                screenPos.y -= tileHeightOffset;

                m_impl->character->SetPosition(screenPos.x, screenPos.y);
            }
        }

        // TileMap updates would go here if needed in the future
        // For now, TileMap is stateless between frames
    }

    void GameplayScene::Render() {
        if (!m_initialized || !m_impl->tileMap || !m_renderer) {
            return;
        }

        // Render the TileMap using the modern renderer interface
        m_impl->tileMap->Render(m_renderer);

        // Render character on top of tilemap
        if (m_impl->character) {
            int x, y;
            m_impl->character->GetPosition(x, y);
            m_impl->character->Render(m_renderer, x, y);
        }
    }

    void GameplayScene::MoveCharacterToTile(uint16_t row, uint16_t col, float duration) {
        if (!m_initialized || !m_impl->character || !m_impl->tileMap) {
            return;
        }

        // Validate tile coordinates
        if (row >= m_impl->tileMap->GetHeight() || col >= m_impl->tileMap->GetWidth()) {
            if (m_logger) {
                m_logger->Warning("Invalid tile coordinates: (" + std::to_string(row) + 
                                ", " + std::to_string(col) + ")");
            }
            return;
        }

        // Start movement
        m_impl->targetRow = row;
        m_impl->targetCol = col;
        m_impl->isMoving = true;
        m_impl->moveTime = 0.0f;
        m_impl->moveDuration = duration > 0.0f ? duration : 0.3f;

        if (m_logger) {
            m_logger->Debug("Moving character from tile (" + std::to_string(m_impl->characterRow) + 
                          ", " + std::to_string(m_impl->characterCol) + ") to (" + 
                          std::to_string(row) + ", " + std::to_string(col) + ")");
        }
    }
}
