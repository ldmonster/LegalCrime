#include "Window.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

namespace Engine {
    
    Window::Window(ILogger* logger)
        : m_logger(logger)
        , m_window(nullptr)
        , m_resolution(800, 600)
        , m_title("Game Window")
        , m_mode(WindowMode::Windowed)
        , m_initialized(false) {
    }
    
    Window::~Window() {
        Shutdown();
    }
    
    Result<void> Window::Initialize(const WindowConfig& config) {
        if (m_initialized) {
            return Result<void>::Failure("Window already initialized");
        }
        
        // Initialize SDL Video subsystem
        if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
            std::string error = "SDL Video could not initialize! SDL Error: ";
            error += SDL_GetError();
            if (m_logger) m_logger->Error(error);
            return Result<void>::Failure(error);
        }
        
        m_title = config.title;
        m_resolution = config.resolution;
        m_mode = config.mode;
        
        uint32 flags = GetSDLWindowFlags(config);
        
        m_window = SDL_CreateWindow(
            m_title.c_str(),
            static_cast<int>(m_resolution.width),
            static_cast<int>(m_resolution.height),
            flags
        );
        
        if (!m_window) {
            std::string error = "Window could not be created! SDL Error: ";
            error += SDL_GetError();
            if (m_logger) m_logger->Error(error);
            return Result<void>::Failure(error);
        }
        
        m_initialized = true;
        
        if (m_logger) {
            m_logger->Debug("Window initialized successfully: " + m_title);
        }
        
        return Result<void>::Success();
    }
    
    void Window::Shutdown() {
        if (m_window) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }
        m_initialized = false;
        
        if (m_logger) {
            m_logger->Debug("Window shutdown");
        }
    }
    
    bool Window::SetIcon(const std::string& iconPath) {
        if (!m_window) {
            if (m_logger) m_logger->Warning("Cannot set icon: Window not initialized");
            return false;
        }
        
        SDL_Surface* iconSurface = IMG_Load(iconPath.c_str());
        if (!iconSurface) {
            if (m_logger) {
                std::string error = "Failed to load icon: ";
                error += SDL_GetError();
                m_logger->Warning(error);
            }
            return false;
        }
        
        SDL_SetWindowIcon(m_window, iconSurface);
        SDL_DestroySurface(iconSurface);
        
        if (m_logger) {
            m_logger->Debug("Window icon set: " + iconPath);
        }
        
        return true;
    }
    
    bool Window::SetTitle(const std::string& title) {
        if (!m_window) {
            if (m_logger) m_logger->Warning("Cannot set title: Window not initialized");
            return false;
        }
        
        m_title = title;
        SDL_SetWindowTitle(m_window, m_title.c_str());
        return true;
    }
    
    bool Window::SetResolution(const Resolution& resolution) {
        if (!m_window) {
            if (m_logger) m_logger->Warning("Cannot set resolution: Window not initialized");
            return false;
        }
        
        m_resolution = resolution;
        SDL_SetWindowSize(m_window, 
                         static_cast<int>(m_resolution.width), 
                         static_cast<int>(m_resolution.height));
        return true;
    }
    
    uint32 Window::GetSDLWindowFlags(const WindowConfig& config) const {
        uint32 flags = 0;
        
        switch (config.mode) {
            case WindowMode::Fullscreen:
                flags |= SDL_WINDOW_FULLSCREEN;
                break;
            case WindowMode::BorderlessFullscreen:
                flags |= SDL_WINDOW_BORDERLESS;
                break;
            case WindowMode::Windowed:
            default:
                break;
        }
        
        if (config.resizable) {
            flags |= SDL_WINDOW_RESIZABLE;
        }
        
        return flags;
    }
}
