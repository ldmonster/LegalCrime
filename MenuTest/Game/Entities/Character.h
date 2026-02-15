#pragma once

#include "../../Engine/Entity/Entity.h"
#include "../../Engine/Graphics/AnimatedSprite.h"
#include "CharacterData.h"
#include <memory>

namespace Engine {
    class IRenderer;
}

namespace LegalCrime {
namespace Entities {

    /// <summary>
    /// Game character entity with sprite rendering and movement.
    /// Combines entity transform with animated sprite rendering.
    /// </summary>
    class Character : public Engine::Entity {
    public:
        Character(
            CharacterType type,
            std::shared_ptr<Engine::Texture> texture,
            const Engine::CharacterSpriteConfig& config,
            Engine::ILogger* logger = nullptr);

        virtual ~Character();

        // Entity lifecycle
        void Update(float deltaTime) override;
        void Render() override;  // Override to render with stored renderer

        // Render with explicit renderer
        void Render(Engine::IRenderer* renderer);

        // Animation control
        bool SetAnimation(const std::string& name);
        const std::string& GetCurrentAnimation() const;

        // Direction control
        void SetDirection(Direction dir);
        Direction GetDirection() const { return m_direction; }

        // Movement
        void MoveTo(int x, int y, float duration = 0.3f);
        void MoveTo(const Engine::Point& pos, float duration = 0.3f);
        bool IsMoving() const;
        void StopMovement();

        // Character data
        CharacterType GetCharacterType() const { return m_characterType; }
        const CharacterData& GetCharacterData() const { return m_data; }

        // Sprite access (for advanced usage)
        Engine::AnimatedSprite* GetSprite() { return m_sprite.get(); }
        const Engine::AnimatedSprite* GetSprite() const { return m_sprite.get(); }

    private:
        CharacterType m_characterType;
        CharacterData m_data;
        Direction m_direction;
        std::unique_ptr<Engine::AnimatedSprite> m_sprite;
        Engine::IRenderer* m_cachedRenderer;  // For parameterless Render()

        void InitializeAnimations(const Engine::CharacterSpriteConfig& config);
        void UpdateDirectionAnimation();
    };

} // namespace Entities
} // namespace LegalCrime
