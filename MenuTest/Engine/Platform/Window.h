#pragma once

#include "IWindow.h"
#include "../Core/Logger/ILogger.h"
#include <SDL3/SDL.h>

namespace Engine {
    
    class Window : public IWindow {
    public:
        explicit Window(ILogger* logger);
        ~Window() override;
        
        Result<void> Initialize(const WindowConfig& config) override;
        void Shutdown() override;
        
        SDL_Window* GetNativeWindow() const override { return m_window; }
        Resolution GetResolution() const override { return m_resolution; }
        const std::string& GetTitle() const override { return m_title; }
        
        bool SetIcon(const std::string& iconPath) override;
        bool SetTitle(const std::string& title) override;
        bool SetResolution(const Resolution& resolution) override;
        
        bool IsInitialized() const override { return m_initialized; }
        
    private:
        ILogger* m_logger;
        SDL_Window* m_window;
        Resolution m_resolution;
        std::string m_title;
        WindowMode m_mode;
        bool m_initialized;
        
        uint32 GetSDLWindowFlags(const WindowConfig& config) const;
    };
}
