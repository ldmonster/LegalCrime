#include "Character.h"
#include "../../Engine/Renderer/IRenderer.h"
#include "../../Engine/Core/Logger/ILogger.h"

namespace LegalCrime {
namespace Entities {

    Character::Character(
        CharacterType type,
        std::shared_ptr<Engine::Texture> texture,
        const Engine::CharacterSpriteConfig& config,
        Engine::ILogger* logger)
        : Entity(CharacterDataUtils::CharacterTypeToString(type), logger)
        , m_characterType(type)
        , m_data()
        , m_direction(Direction::Down)
        , m_sprite(nullptr)
        , m_cachedRenderer(nullptr) {

        m_data.type = type;
        m_data.name = CharacterDataUtils::CharacterTypeToString(type);
        m_data.spriteConfig = config;

        // Set default scale from config
        SetScale(config.defaultScale);

        if (texture) {
            m_sprite = std::make_unique<Engine::AnimatedSprite>(texture, logger);
            m_sprite->SetScale(GetScale());
            
            // Initialize animations
            InitializeAnimations(config);
            
            // Set default animation
            if (!config.defaultAnimation.empty()) {
                SetAnimation(config.defaultAnimation);
            }

            if (m_logger) {
                m_logger->Info("Character created: " + m_data.name + 
                             " with " + std::to_string(config.animations.size()) + " animations");
            }
        } else {
            if (m_logger) {
                m_logger->Error("Character: Null texture provided for " + m_data.name);
            }
        }
    }

    Character::~Character() {
    }

    void Character::InitializeAnimations(const Engine::CharacterSpriteConfig& config) {
        if (!m_sprite) return;

        for (const auto& animConfig : config.animations) {
            // Create animation using helper function
            auto animation = Engine::CreateGridAnimation(
                animConfig.name,
                animConfig.frameWidth,
                animConfig.frameHeight,
                animConfig.sheetColumns,
                animConfig.sheetRows,
                animConfig.startFrame,
                animConfig.endFrame,
                animConfig.frameDuration,
                animConfig.loop
            );

            m_sprite->AddAnimation(animation);

            if (m_logger) {
                m_logger->Debug("Character (" + m_data.name + "): Added animation '" + 
                              animConfig.name + "' (frames " + 
                              std::to_string(animConfig.startFrame) + "-" + 
                              std::to_string(animConfig.endFrame) + ")");
            }
        }
    }

    void Character::Update(float deltaTime) {
        if (m_sprite) {
            // Sync sprite position with entity transform
            m_sprite->SetPosition(m_transform.position.x, m_transform.position.y);
            
            // Update sprite animation
            m_sprite->Update(deltaTime);

            // If sprite is moving, sync transform back
            if (m_sprite->IsMoving()) {
                int x, y;
                m_sprite->GetPosition(x, y);
                m_transform.SetPosition(x, y);
            }
        }
    }

    void Character::Render() {
        if (m_cachedRenderer && m_sprite) {
            m_sprite->Render(m_cachedRenderer, m_transform.position.x, m_transform.position.y);
        }
    }

    void Character::Render(Engine::IRenderer* renderer) {
        m_cachedRenderer = renderer;  // Cache for parameterless Render()
        
        if (renderer && m_sprite) {
            m_sprite->Render(renderer, m_transform.position.x, m_transform.position.y);
        }
    }

    bool Character::SetAnimation(const std::string& name) {
        if (!m_sprite) {
            return false;
        }

        m_sprite->SetAnimation(name);
        return true;
    }

    const std::string& Character::GetCurrentAnimation() const {
        static const std::string empty = "";
        if (m_sprite) {
            return m_sprite->GetCurrentAnimationName();
        }
        return empty;
    }

    void Character::SetDirection(Direction dir) {
        if (m_direction != dir) {
            m_direction = dir;
            UpdateDirectionAnimation();
        }
    }

    void Character::UpdateDirectionAnimation() {
        // Update animation based on direction
        std::string animName = CharacterDataUtils::GetWalkAnimationForDirection(m_direction);
        SetAnimation(animName);
    }

    void Character::MoveTo(int x, int y, float duration) {
        if (m_sprite) {
            m_sprite->MoveTo(x, y, duration);
        }
        
        // Movement will be synced in Update()
    }

    void Character::MoveTo(const Engine::Point& pos, float duration) {
        MoveTo(pos.x, pos.y, duration);
    }

    bool Character::IsMoving() const {
        return m_sprite ? m_sprite->IsMoving() : false;
    }

    void Character::StopMovement() {
        if (m_sprite) {
            m_sprite->StopMovement();
            
            // Sync position back to transform
            int x, y;
            m_sprite->GetPosition(x, y);
            m_transform.SetPosition(x, y);
        }
    }

} // namespace Entities
} // namespace LegalCrime
