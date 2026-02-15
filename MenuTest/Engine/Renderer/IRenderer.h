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
    };
}
