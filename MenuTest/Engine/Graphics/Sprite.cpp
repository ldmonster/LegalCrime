#include "Sprite.h"
#include "../Renderer/IRenderer.h"

namespace Engine {
    
    Sprite::Sprite()
        : m_texture(nullptr)
        , m_sourceRect()
        , m_hasSourceRect(false) {
    }
    
    Sprite::Sprite(std::shared_ptr<Texture> texture)
        : m_texture(texture)
        , m_sourceRect()
        , m_hasSourceRect(false) {
    }
    
    Sprite::Sprite(std::shared_ptr<Texture> texture, const Rect& sourceRect)
        : m_texture(texture)
        , m_sourceRect(sourceRect)
        , m_hasSourceRect(true) {
    }
    
    void Sprite::Render(
        IRenderer* renderer,
        const Rect& destRect,
        double angle,
        const SDL_FPoint* center,
        SDL_FlipMode flip
    ) {
        if (!m_texture || !renderer) {
            return;
        }
        
        const Rect* srcRect = m_hasSourceRect ? &m_sourceRect : nullptr;
        m_texture->Render(renderer, destRect, srcRect, angle, center, flip);
    }
    
    void Sprite::SetTexture(std::shared_ptr<Texture> texture) {
        m_texture = texture;
    }
    
    void Sprite::SetColorMod(const Color& color) {
        if (m_texture) {
            m_texture->SetColorMod(color);
        }
    }
    
    void Sprite::SetAlphaMod(uint8_t alpha) {
        if (m_texture) {
            m_texture->SetAlphaMod(alpha);
        }
    }
    
    int Sprite::GetWidth() const {
        if (!m_texture) return 0;
        return m_hasSourceRect ? m_sourceRect.w : m_texture->GetWidth();
    }
    
    int Sprite::GetHeight() const {
        if (!m_texture) return 0;
        return m_hasSourceRect ? m_sourceRect.h : m_texture->GetHeight();
    }
}
