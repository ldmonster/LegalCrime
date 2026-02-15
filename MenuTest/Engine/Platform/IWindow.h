#pragma once

#include "../Core/Types.h"
#include <string>

struct SDL_Window;

namespace Engine {
    
    class ILogger;
    
    enum class WindowMode {
        Windowed,
        Fullscreen,
        BorderlessFullscreen
    };
    
    struct WindowConfig {
        std::string title;
        Resolution resolution;
        WindowMode mode;
        bool resizable;
        bool vSync;
        
        WindowConfig()
            : title("Game Window")
            , resolution(800, 600)
            , mode(WindowMode::Windowed)
            , resizable(false)
            , vSync(false) {}
    };
    
    // Abstract window interface
    class IWindow {
    public:
        virtual ~IWindow() = default;
        
        virtual Result<void> Initialize(const WindowConfig& config) = 0;
        virtual void Shutdown() = 0;
        
        virtual SDL_Window* GetNativeWindow() const = 0;
        virtual Resolution GetResolution() const = 0;
        virtual const std::string& GetTitle() const = 0;
        
        virtual bool SetIcon(const std::string& iconPath) = 0;
        virtual bool SetTitle(const std::string& title) = 0;
        virtual bool SetResolution(const Resolution& resolution) = 0;
        
        virtual bool IsInitialized() const = 0;
    };
}
