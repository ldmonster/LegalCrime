#pragma once

#include "../Core/Types.h"
#include <SDL3/SDL.h>
#include <string>
#include <memory>

namespace Engine {

    class IRenderer;
    class ILogger;

    // SDL conversion utilities for Rect (extension functions)
    inline SDL_Rect RectToSDL(const Rect& rect) {
        return SDL_Rect{rect.x, rect.y, rect.w, rect.h};
    }

    inline SDL_FRect RectToSDLFloat(const Rect& rect) {
        return SDL_FRect{(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h};
    }

    inline Rect RectFromSDL(const SDL_Rect& rect) {
        return Rect(rect.x, rect.y, rect.w, rect.h);
    }

    // Modern texture class with RAII
    class Texture {
    public:
        // Factory method for loading from file
        static std::shared_ptr<Texture> LoadFromFile(
            IRenderer* renderer,
            const std::string& path,
            ILogger* logger = nullptr
        );
        
        // Factory method for creating from rendered text
        static std::shared_ptr<Texture> LoadFromText(
            IRenderer* renderer,
            const std::string& text,
            const std::string& fontPath,
            int fontSize,
            const Color& color,
            ILogger* logger = nullptr
        );
        
        // Create blank texture for rendering target
        static std::shared_ptr<Texture> CreateBlank(
            IRenderer* renderer,
            int width,
            int height,
            ILogger* logger = nullptr
        );

        // Create from existing SDL_Texture (takes ownership)
        static std::shared_ptr<Texture> CreateFromSDL(
            SDL_Texture* texture,
            int width,
            int height
        );

        ~Texture();
        
        // Disable copy, enable move
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;
        
        // Rendering
        void Render(
            IRenderer* renderer,
            const Rect& destRect,
            const Rect* sourceRect = nullptr,
            double angle = 0.0,
            const SDL_FPoint* center = nullptr,
            SDL_FlipMode flip = SDL_FLIP_NONE
        );
        
        // Modulation
        void SetColorMod(const Color& color);
        void SetAlphaMod(uint8_t alpha);
        void SetBlendMode(SDL_BlendMode blending);
        
        // Accessors
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        SDL_Texture* GetSDLTexture() const { return m_texture; }
        
    private:
        Texture(SDL_Texture* texture, int width, int height);
        
        SDL_Texture* m_texture;
        int m_width;
        int m_height;
    };
}
