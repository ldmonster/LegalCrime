#pragma once

#include "../Core/Types.h"
#include "../Platform/IWindow.h"

struct SDL_Renderer;

namespace Engine {
    
    class ILogger;
    
    struct RendererConfig {
        bool vSync;
        bool accelerated;
        
        RendererConfig()
            : vSync(false)
            , accelerated(true) {}
    };
    
    // Abstract renderer interface
    class IRenderer {
    public:
        virtual ~IRenderer() = default;
        
        virtual Result<void> Initialize(IWindow* window, const RendererConfig& config) = 0;
        virtual void Shutdown() = 0;
        
        virtual void Clear(const Color& color = Color(0, 0, 0, 255)) = 0;
        virtual void Present() = 0;
        
        virtual SDL_Renderer* GetNativeRenderer() const = 0;
        
        virtual bool SetVSync(bool enabled) = 0;
        virtual bool IsVSyncEnabled() const = 0;
        
        virtual bool IsInitialized() const = 0;

        // Drawing primitives — game code should use these instead of GetNativeRenderer()
        virtual void SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;
        virtual void DrawLines(const Point* points, int count) = 0;
        virtual void DrawCircle(const Point& center, int radius, int segments = 32) = 0;
    };
}
