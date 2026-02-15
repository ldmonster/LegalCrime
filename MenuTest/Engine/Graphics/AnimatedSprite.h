#pragma once

#include "Sprite.h"
#include "../Core/Types.h"
#include <memory>
#include <vector>
#include <string>

namespace Engine {
    
    class ILogger;
    
    // Represents a single frame in an animation
    struct AnimationFrame {
        Rect sourceRect;        // Rectangle in the sprite sheet
        float duration;         // Duration in seconds
        
        AnimationFrame() : sourceRect(), duration(0.1f) {}
        AnimationFrame(const Rect& rect, float dur = 0.1f) 
            : sourceRect(rect), duration(dur) {}
    };
    
    // Represents an animation sequence
    struct Animation {
        std::string name;
        std::vector<AnimationFrame> frames;
        bool loop;
        
        Animation() : name(""), loop(true) {}
        Animation(const std::string& n, bool l = true) 
            : name(n), loop(l) {}
    };
    
    // Animated sprite class for rendering animated characters
    class AnimatedSprite {
    public:
        AnimatedSprite(std::shared_ptr<Texture> texture, ILogger* logger = nullptr);
        ~AnimatedSprite();
        
        // Animation management
        void AddAnimation(const Animation& animation);
        void SetAnimation(const std::string& name);
        void Update(float deltaTime);
        
        // Rendering
        void Render(IRenderer* renderer, int x, int y);
        void Render(IRenderer* renderer, const Rect& destRect);
        
        // Position and properties
        void SetPosition(int x, int y) { m_x = x; m_y = y; m_targetX = x; m_targetY = y; m_isMoving = false; }
        void GetPosition(int& x, int& y) const { x = m_x; y = m_y; }

        // Smooth movement
        void MoveTo(int x, int y, float duration = 0.3f);
        bool IsMoving() const { return m_isMoving; }
        void StopMovement() { m_isMoving = false; m_x = m_targetX; m_y = m_targetY; }
        float GetMoveProgress() const { return m_isMoving ? (m_moveTime / m_moveDuration) : 1.0f; }

        void SetScale(float scale) { m_scale = scale; }
        float GetScale() const { return m_scale; }
        
        // Flip
        void SetFlip(SDL_FlipMode flip) { m_flip = flip; }
        SDL_FlipMode GetFlip() const { return m_flip; }
        
        // Current frame info
        const AnimationFrame* GetCurrentFrame() const;
        std::string GetCurrentAnimationName() const;
        
    private:
        std::shared_ptr<Texture> m_texture;
        ILogger* m_logger;
        
        // Animations
        std::vector<Animation> m_animations;
        int m_currentAnimation;
        int m_currentFrame;
        float m_frameTime;
        
        // Position and scale
        int m_x, m_y;
        float m_scale;
        SDL_FlipMode m_flip;

        // Smooth movement
        bool m_isMoving;
        int m_startX, m_startY;
        int m_targetX, m_targetY;
        float m_moveTime;
        float m_moveDuration;
    };
    
    // Helper function to create a grid-based sprite sheet animation
    // (e.g., for a 4x4 grid of 32x32 frames)
    Animation CreateGridAnimation(
        const std::string& name,
        int frameWidth,
        int frameHeight,
        int columns,
        int rows,
        int startFrame = 0,
        int frameCount = -1,  // -1 means all frames
        float frameDuration = 0.1f,
        bool loop = true
    );
}
