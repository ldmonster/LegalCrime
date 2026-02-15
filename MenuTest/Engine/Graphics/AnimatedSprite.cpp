#include "AnimatedSprite.h"
#include "../Renderer/IRenderer.h"
#include "../Core/Logger/ILogger.h"

namespace Engine {
    
    AnimatedSprite::AnimatedSprite(std::shared_ptr<Texture> texture, ILogger* logger)
        : m_texture(texture)
        , m_logger(logger)
        , m_currentAnimation(-1)
        , m_currentFrame(0)
        , m_frameTime(0.0f)
        , m_x(0)
        , m_y(0)
        , m_scale(1.0f)
        , m_flip(SDL_FLIP_NONE)
        , m_isMoving(false)
        , m_startX(0)
        , m_startY(0)
        , m_targetX(0)
        , m_targetY(0)
        , m_moveTime(0.0f)
        , m_moveDuration(0.3f) {
    }
    
    AnimatedSprite::~AnimatedSprite() {
    }
    
    void AnimatedSprite::AddAnimation(const Animation& animation) {
        m_animations.push_back(animation);
        
        // If this is the first animation, set it as current
        if (m_currentAnimation == -1 && !m_animations.empty()) {
            m_currentAnimation = 0;
            m_currentFrame = 0;
            m_frameTime = 0.0f;
        }
        
        if (m_logger) {
            m_logger->Debug("Added animation '" + animation.name + "' with " + 
                          std::to_string(animation.frames.size()) + " frames");
        }
    }
    
    void AnimatedSprite::SetAnimation(const std::string& name) {
        for (size_t i = 0; i < m_animations.size(); i++) {
            if (m_animations[i].name == name) {
                if (m_currentAnimation != static_cast<int>(i)) {
                    m_currentAnimation = static_cast<int>(i);
                    m_currentFrame = 0;
                    m_frameTime = 0.0f;
                    
                    if (m_logger) {
                        m_logger->Debug("Set animation to '" + name + "'");
                    }
                }
                return;
            }
        }
        
        if (m_logger) {
            m_logger->Warning("Animation '" + name + "' not found");
        }
    }
    
    void AnimatedSprite::Update(float deltaTime) {
        // Update smooth movement interpolation
        if (m_isMoving) {
            m_moveTime += deltaTime;

            if (m_moveTime >= m_moveDuration) {
                // Movement complete
                m_x = m_targetX;
                m_y = m_targetY;
                m_isMoving = false;
                m_moveTime = 0.0f;
            } else {
                // Interpolate position (linear interpolation)
                float t = m_moveTime / m_moveDuration;
                m_x = static_cast<int>(m_startX + (m_targetX - m_startX) * t);
                m_y = static_cast<int>(m_startY + (m_targetY - m_startY) * t);
            }
        }

        if (m_currentAnimation < 0 || m_currentAnimation >= static_cast<int>(m_animations.size())) {
            return;
        }

        const Animation& anim = m_animations[m_currentAnimation];
        if (anim.frames.empty()) {
            return;
        }

        m_frameTime += deltaTime;

        const AnimationFrame& currentFrame = anim.frames[m_currentFrame];
        if (m_frameTime >= currentFrame.duration) {
            m_frameTime -= currentFrame.duration;
            m_currentFrame++;

            // Handle loop/end
            if (m_currentFrame >= static_cast<int>(anim.frames.size())) {
                if (anim.loop) {
                    m_currentFrame = 0;
                } else {
                    m_currentFrame = static_cast<int>(anim.frames.size()) - 1;
                }
            }
        }
    }
    
    void AnimatedSprite::Render(IRenderer* renderer, int x, int y) {
        const AnimationFrame* frame = GetCurrentFrame();
        if (!frame || !m_texture || !renderer) {
            return;
        }
        
        // Calculate destination rectangle with scale
        int width = static_cast<int>(frame->sourceRect.w * m_scale);
        int height = static_cast<int>(frame->sourceRect.h * m_scale);
        
        Rect destRect(x, y, width, height);
        
        m_texture->Render(renderer, destRect, &frame->sourceRect, 0.0, nullptr, m_flip);
    }
    
    void AnimatedSprite::Render(IRenderer* renderer, const Rect& destRect) {
        const AnimationFrame* frame = GetCurrentFrame();
        if (!frame || !m_texture || !renderer) {
            return;
        }
        
        m_texture->Render(renderer, destRect, &frame->sourceRect, 0.0, nullptr, m_flip);
    }
    
    const AnimationFrame* AnimatedSprite::GetCurrentFrame() const {
        if (m_currentAnimation < 0 || m_currentAnimation >= static_cast<int>(m_animations.size())) {
            return nullptr;
        }
        
        const Animation& anim = m_animations[m_currentAnimation];
        if (m_currentFrame < 0 || m_currentFrame >= static_cast<int>(anim.frames.size())) {
            return nullptr;
        }
        
        return &anim.frames[m_currentFrame];
    }
    
    std::string AnimatedSprite::GetCurrentAnimationName() const {
        if (m_currentAnimation >= 0 && m_currentAnimation < static_cast<int>(m_animations.size())) {
            return m_animations[m_currentAnimation].name;
        }
        return "";
    }

    void AnimatedSprite::MoveTo(int x, int y, float duration) {
        if (m_x == x && m_y == y) {
            // Already at target position
            return;
        }

        m_startX = m_x;
        m_startY = m_y;
        m_targetX = x;
        m_targetY = y;
        m_moveTime = 0.0f;
        m_moveDuration = duration > 0.0f ? duration : 0.3f;
        m_isMoving = true;

        if (m_logger) {
            m_logger->Debug("Moving sprite from (" + std::to_string(m_startX) + ", " + 
                          std::to_string(m_startY) + ") to (" + std::to_string(m_targetX) + 
                          ", " + std::to_string(m_targetY) + ") over " + 
                          std::to_string(m_moveDuration) + " seconds");
        }
    }
    
    // Helper function to create grid-based animation
    Animation CreateGridAnimation(
        const std::string& name,
        int frameWidth,
        int frameHeight,
        int columns,
        int rows,
        int startFrame,
        int frameCount,
        float frameDuration,
        bool loop
    ) {
        Animation anim(name, loop);
        
        int totalFrames = columns * rows;
        if (frameCount < 0) {
            frameCount = totalFrames - startFrame;
        }
        
        for (int i = 0; i < frameCount; i++) {
            int frameIndex = startFrame + i;
            if (frameIndex >= totalFrames) break;
            
            int col = frameIndex % columns;
            int row = frameIndex / columns;
            
            Rect sourceRect(
                col * frameWidth,
                row * frameHeight,
                frameWidth,
                frameHeight
            );
            
            anim.frames.push_back(AnimationFrame(sourceRect, frameDuration));
        }
        
        return anim;
    }
}
