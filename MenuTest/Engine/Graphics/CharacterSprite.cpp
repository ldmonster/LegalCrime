#include "CharacterSprite.h"

namespace Engine {

    CharacterSprite::CharacterSprite(
        const std::string& characterType,
        std::shared_ptr<Texture> texture,
        float defaultScale,
        ILogger* logger)
        : m_characterType(characterType)
        , m_sprite(nullptr)
        , m_scale(defaultScale)
        , m_x(0)
        , m_y(0)
        , m_logger(logger) {

        if (texture) {
            m_sprite = std::make_unique<AnimatedSprite>(texture, logger);
            m_sprite->SetScale(m_scale);
        } else {
            if (m_logger) {
                m_logger->Error("CharacterSprite: Null texture provided for " + characterType);
            }
        }
    }

    CharacterSprite::~CharacterSprite() {
    }

    void CharacterSprite::AddAnimation(const AnimationConfig& config) {
        if (!m_sprite) {
            if (m_logger) {
                m_logger->Error("CharacterSprite: Cannot add animation, sprite not initialized");
            }
            return;
        }

        // Create animation using the CreateGridAnimation helper
        auto animation = CreateGridAnimation(
            config.name,
            config.frameWidth,
            config.frameHeight,
            config.sheetColumns,
            config.sheetRows,
            config.startFrame,
            config.endFrame,
            config.frameDuration,
            config.loop
        );

        m_sprite->AddAnimation(animation);

        if (m_logger) {
            m_logger->Debug("CharacterSprite (" + m_characterType + "): Added animation '" + 
                          config.name + "' (frames " + std::to_string(config.startFrame) + 
                          "-" + std::to_string(config.endFrame) + ")");
        }
    }

    bool CharacterSprite::SetAnimation(const std::string& name) {
        if (!m_sprite) {
            return false;
        }

        m_sprite->SetAnimation(name);
        return true;
    }

    void CharacterSprite::Update(float deltaTime) {
        if (m_sprite) {
            m_sprite->Update(deltaTime);
        }
    }

    void CharacterSprite::Render(IRenderer* renderer, int x, int y) {
        if (m_sprite) {
            m_sprite->Render(renderer, x, y);
        }
    }

    void CharacterSprite::SetPosition(int x, int y) {
        m_x = x;
        m_y = y;
    }

    void CharacterSprite::GetPosition(int& x, int& y) const {
        x = m_x;
        y = m_y;
    }

    void CharacterSprite::SetScale(float scale) {
        m_scale = scale;
        if (m_sprite) {
            m_sprite->SetScale(scale);
        }
    }

    float CharacterSprite::GetScale() const {
        return m_scale;
    }

    const std::string& CharacterSprite::GetCurrentAnimation() const {
        static const std::string empty = "";
        if (m_sprite) {
            return m_sprite->GetCurrentAnimationName();
        }
        return empty;
    }

} // namespace Engine
