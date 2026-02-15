#pragma once

#include "Texture.h"
#include <memory>

namespace Engine {
    
    class IRenderer;
    
    // Modern sprite class using shared textures
    class Sprite {
    public:
        Sprite();
        Sprite(std::shared_ptr<Texture> texture);
        Sprite(std::shared_ptr<Texture> texture, const Rect& sourceRect);
        ~Sprite() = default;
        
        // Copy allowed (sprites share textures)
        Sprite(const Sprite&) = default;
        Sprite& operator=(const Sprite&) = default;
        
        // Rendering
        void Render(
            IRenderer* renderer,
            const Rect& destRect,
            double angle = 0.0,
            const SDL_FPoint* center = nullptr,
            SDL_FlipMode flip = SDL_FLIP_NONE
        );
        
        // Texture management
        void SetTexture(std::shared_ptr<Texture> texture);
        std::shared_ptr<Texture> GetTexture() const { return m_texture; }
        
        // Source rectangle (what part of texture to draw)
        void SetSourceRect(const Rect& rect) { m_sourceRect = rect; m_hasSourceRect = true; }
        const Rect& GetSourceRect() const { return m_sourceRect; }
        void ClearSourceRect() { m_hasSourceRect = false; }
        bool HasSourceRect() const { return m_hasSourceRect; }
        
        // Color/Alpha modulation
        void SetColorMod(const Color& color);
        void SetAlphaMod(uint8_t alpha);
        
        // Convenience accessors
        int GetWidth() const;
        int GetHeight() const;
        
    private:
        std::shared_ptr<Texture> m_texture;
        Rect m_sourceRect;
        bool m_hasSourceRect;
    };
}
