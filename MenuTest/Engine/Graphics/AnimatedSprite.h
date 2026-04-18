#pragma once

#include "Sprite.h"
#include "../Core/Types.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

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

    // Smooth movement interpolation utility.
    // Owned by entities that need position interpolation (e.g., Character).
    // Writes interpolated position to the provided Transform (single source of truth).
    struct SmoothMovement {
        bool isMoving = false;
        int startX = 0, startY = 0;
        int targetX = 0, targetY = 0;
        float moveTime = 0.0f;
        float moveDuration = 0.3f;

        void Start(int fromX, int fromY, int toX, int toY, float duration = 0.3f) {
            if (fromX == toX && fromY == toY) return;
            startX = fromX;
            startY = fromY;
            targetX = toX;
            targetY = toY;
            moveTime = 0.0f;
            moveDuration = duration > 0.0f ? duration : 0.3f;
            isMoving = true;
        }

        // Returns true while still moving. Updates outX/outY with interpolated position.
        bool Update(float deltaTime, int& outX, int& outY) {
            if (!isMoving) return false;
            moveTime += deltaTime;
            if (moveTime >= moveDuration) {
                outX = targetX;
                outY = targetY;
                isMoving = false;
                moveTime = 0.0f;
                return false; // movement finished
            }
            float t = moveTime / moveDuration;
            outX = static_cast<int>(startX + (targetX - startX) * t);
            outY = static_cast<int>(startY + (targetY - startY) * t);
            return true; // still moving
        }

        void Stop(int& outX, int& outY) {
            outX = targetX;
            outY = targetY;
            isMoving = false;
            moveTime = 0.0f;
        }

        float GetProgress() const { return isMoving ? (moveTime / moveDuration) : 1.0f; }
    };
    
    // Animated sprite class — handles animation frames only.
    // Does NOT own position. Position is provided at render time by the owning entity's Transform.
    class AnimatedSprite {
    public:
        AnimatedSprite(std::shared_ptr<Texture> texture, ILogger* logger = nullptr);
        ~AnimatedSprite();
        
        // Animation management
        void AddAnimation(const Animation& animation);
        void SetAnimation(const std::string& name);
        void Update(float deltaTime);
        
        // Rendering — position supplied by caller (from Transform)
        void Render(IRenderer* renderer, int x, int y);
        void Render(IRenderer* renderer, const Rect& destRect);

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
        std::unordered_map<std::string, int> m_animationIndex; // O(1) name→index lookup
        int m_currentAnimation;
        int m_currentFrame;
        float m_frameTime;
        
        // Scale and flip
        float m_scale;
        SDL_FlipMode m_flip;
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
