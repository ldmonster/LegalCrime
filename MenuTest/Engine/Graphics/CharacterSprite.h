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

        // Render the character at the specified position
        void Render(IRenderer* renderer, int x, int y);

        // Position management
        void SetPosition(int x, int y);
        void GetPosition(int& x, int& y) const;

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
        int m_x;
        int m_y;
        ILogger* m_logger;
    };

} // namespace Engine
