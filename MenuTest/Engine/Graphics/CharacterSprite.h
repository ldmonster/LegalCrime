#pragma once

#include "AnimatedSprite.h"
#include "CharacterSpriteConfig.h"
#include "Texture.h"
#include "../Core/Logger/ILogger.h"
#include <memory>
#include <string>

namespace Engine {

    /// <summary>
    /// Represents a character sprite with animations loaded from configuration.
    /// Wraps AnimatedSprite and provides a higher-level interface for character management.
    /// Does NOT own position — position is provided at render time by the owning entity's Transform.
    /// </summary>
    class CharacterSprite {
    public:
        CharacterSprite(
            const std::string& characterType,
            std::shared_ptr<Texture> texture,
            float defaultScale,
            ILogger* logger = nullptr);

        ~CharacterSprite();

        // Add an animation to this character
        void AddAnimation(const AnimationConfig& config);

        // Set the current animation by name
        bool SetAnimation(const std::string& name);

        // Update animation (call each frame)
        void Update(float deltaTime);

        // Render the character at the specified position (from Transform)
        void Render(IRenderer* renderer, int x, int y);

        // Scale management
        void SetScale(float scale);
        float GetScale() const;

        // Get character type identifier
        const std::string& GetCharacterType() const { return m_characterType; }

        // Get current animation name
        const std::string& GetCurrentAnimation() const;

    private:
        std::string m_characterType;
        std::unique_ptr<AnimatedSprite> m_sprite;
        float m_scale;
        ILogger* m_logger;
    };

} // namespace Engine
