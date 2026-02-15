#include "Texture.h"
#include "../Renderer/IRenderer.h"
#include "../Core/Logger/ILogger.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace Engine {

    // Helper to convert Engine::Color to SDL_Color
    static SDL_Color ToSDLColor(const Color& color) {
        return SDL_Color{color.r, color.g, color.b, color.a};
    }
    
    Texture::Texture(SDL_Texture* texture, int width, int height)
        : m_texture(texture)
        , m_width(width)
        , m_height(height) {
    }
    
    Texture::~Texture() {
        if (m_texture) {
            SDL_DestroyTexture(m_texture);
            m_texture = nullptr;
        }
    }
    
    Texture::Texture(Texture&& other) noexcept
        : m_texture(other.m_texture)
        , m_width(other.m_width)
        , m_height(other.m_height) {
        other.m_texture = nullptr;
        other.m_width = 0;
        other.m_height = 0;
    }
    
    Texture& Texture::operator=(Texture&& other) noexcept {
        if (this != &other) {
            if (m_texture) {
                SDL_DestroyTexture(m_texture);
            }
            
            m_texture = other.m_texture;
            m_width = other.m_width;
            m_height = other.m_height;
            
            other.m_texture = nullptr;
            other.m_width = 0;
            other.m_height = 0;
        }
        return *this;
    }
    
    std::shared_ptr<Texture> Texture::LoadFromFile(
        IRenderer* renderer,
        const std::string& path,
        ILogger* logger
    ) {
        if (!renderer) {
            if (logger) logger->Error("LoadFromFile: renderer is null");
            return nullptr;
        }
        
        // Load image surface
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            if (logger) {
                logger->Error("Failed to load image: " + path + " - " + SDL_GetError());
            }
            return nullptr;
        }

        // Create texture from surface
        SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(
            renderer->GetNativeRenderer(),
            surface
        );

        int width = surface->w;
        int height = surface->h;

        SDL_DestroySurface(surface);
        
        if (!sdlTexture) {
            if (logger) {
                logger->Error("Failed to create texture from: " + path + " - " + SDL_GetError());
            }
            return nullptr;
        }
        
        if (logger) {
            logger->Debug("Loaded texture: " + path + " (" + 
                         std::to_string(width) + "x" + std::to_string(height) + ")");
        }
        
        return std::shared_ptr<Texture>(new Texture(sdlTexture, width, height));
    }
    
    std::shared_ptr<Texture> Texture::LoadFromText(
        IRenderer* renderer,
        const std::string& text,
        const std::string& fontPath,
        int fontSize,
        const Color& color,
        ILogger* logger
    ) {
        if (!renderer) {
            if (logger) logger->Error("LoadFromText: renderer is null");
            return nullptr;
        }
        
        // Load font
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            if (logger) {
                logger->Error("Failed to load font: " + fontPath + " - " + SDL_GetError());
            }
            return nullptr;
        }
        
        // Render text to surface
        SDL_Color sdlColor = ToSDLColor(color);
        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), 0, sdlColor);
        
        TTF_CloseFont(font);

        if (!surface) {
            if (logger) {
                logger->Error("Failed to render text: " + text + " - " + SDL_GetError());
            }
            return nullptr;
        }

        // Create texture from surface
        SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(
            renderer->GetNativeRenderer(),
            surface
        );

        int width = surface->w;
        int height = surface->h;

        SDL_DestroySurface(surface);

        if (!sdlTexture) {
            if (logger) {
                logger->Error("Failed to create texture from text - " + std::string(SDL_GetError()));
            }
            return nullptr;
        }

        return std::shared_ptr<Texture>(new Texture(sdlTexture, width, height));
    }
    
    std::shared_ptr<Texture> Texture::CreateBlank(
        IRenderer* renderer,
        int width,
        int height,
        ILogger* logger
    ) {
        if (!renderer) {
            if (logger) logger->Error("CreateBlank: renderer is null");
            return nullptr;
        }
        
        SDL_Texture* sdlTexture = SDL_CreateTexture(
            renderer->GetNativeRenderer(),
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            width,
            height
        );
        
        if (!sdlTexture) {
            if (logger) {
                logger->Error("Failed to create blank texture - " + std::string(SDL_GetError()));
            }
            return nullptr;
        }
        
        return std::shared_ptr<Texture>(new Texture(sdlTexture, width, height));
    }

    std::shared_ptr<Texture> Texture::CreateFromSDL(
        SDL_Texture* texture,
        int width,
        int height
    ) {
        if (!texture) {
            return nullptr;
        }

        return std::shared_ptr<Texture>(new Texture(texture, width, height));
    }

    void Texture::Render(
        IRenderer* renderer,
        const Rect& destRect,
        const Rect* sourceRect,
        double angle,
        const SDL_FPoint* center,
        SDL_FlipMode flip
    ) {
        if (!m_texture || !renderer) {
            return;
        }

        SDL_FRect dest = RectToSDLFloat(destRect);
        SDL_FRect* src = nullptr;
        SDL_FRect srcFloat;

        if (sourceRect) {
            srcFloat = RectToSDLFloat(*sourceRect);
            src = &srcFloat;
        }

        SDL_RenderTextureRotated(
            renderer->GetNativeRenderer(),
            m_texture,
            src,
            &dest,
            angle,
            center,
            flip
        );
    }
    
    void Texture::SetColorMod(const Color& color) {
        if (m_texture) {
            SDL_SetTextureColorMod(m_texture, color.r, color.g, color.b);
        }
    }
    
    void Texture::SetAlphaMod(uint8_t alpha) {
        if (m_texture) {
            SDL_SetTextureAlphaMod(m_texture, alpha);
        }
    }
    
    void Texture::SetBlendMode(SDL_BlendMode blending) {
        if (m_texture) {
            SDL_SetTextureBlendMode(m_texture, blending);
        }
    }
}
