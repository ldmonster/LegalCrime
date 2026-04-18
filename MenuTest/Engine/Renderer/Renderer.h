#pragma once

#include "IRenderer.h"
#include "../Core/Logger/ILogger.h"
#include <SDL3/SDL.h>

namespace Engine {
    
    class Renderer : public IRenderer {
    public:
        explicit Renderer(ILogger* logger);
        ~Renderer() override;
        
        Result<void> Initialize(IWindow* window, const RendererConfig& config) override;
        void Shutdown() override;
        
        void Clear(const Color& color = Color(0, 0, 0, 255)) override;
        void Present() override;
        
        SDL_Renderer* GetNativeRenderer() const override { return m_renderer; }
        
        bool SetVSync(bool enabled) override;
        bool IsVSyncEnabled() const override { return m_vSyncEnabled; }
        
        bool IsInitialized() const override { return m_initialized; }

        void SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
        void DrawLines(const Point* points, int count) override;
        
    private:
        ILogger* m_logger;
        SDL_Renderer* m_renderer;
        IWindow* m_window;
        bool m_vSyncEnabled;
        bool m_initialized;
    };
}
