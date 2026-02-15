#include "Renderer.h"
#include <SDL3/SDL.h>

namespace Engine {
    
    Renderer::Renderer(ILogger* logger)
        : m_logger(logger)
        , m_renderer(nullptr)
        , m_window(nullptr)
        , m_vSyncEnabled(false)
        , m_initialized(false) {
    }
    
    Renderer::~Renderer() {
        Shutdown();
    }
    
    Result<void> Renderer::Initialize(IWindow* window, const RendererConfig& config) {
        if (m_initialized) {
            return Result<void>::Failure("Renderer already initialized");
        }
        
        if (!window || !window->IsInitialized()) {
            std::string error = "Cannot initialize renderer: Invalid or uninitialized window";
            if (m_logger) m_logger->Error(error);
            return Result<void>::Failure(error);
        }
        
        m_window = window;
        m_vSyncEnabled = config.vSync;
        
        m_renderer = SDL_CreateRenderer(window->GetNativeWindow(), nullptr);
        
        if (!m_renderer) {
            std::string error = "Renderer could not be created! SDL Error: ";
            error += SDL_GetError();
            if (m_logger) m_logger->Error(error);
            return Result<void>::Failure(error);
        }
        
        // Set VSync if requested
        if (config.vSync) {
            if (!SDL_SetRenderVSync(m_renderer, 1)) {
                if (m_logger) {
                    std::string warning = "Failed to enable VSync: ";
                    warning += SDL_GetError();
                    m_logger->Warning(warning);
                }
                m_vSyncEnabled = false;
            }
        }
        
        m_initialized = true;
        
        if (m_logger) {
            m_logger->Debug("Renderer initialized successfully");
        }
        
        return Result<void>::Success();
    }
    
    void Renderer::Shutdown() {
        if (m_renderer) {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }
        m_initialized = false;
        m_window = nullptr;
        
        if (m_logger) {
            m_logger->Debug("Renderer shutdown");
        }
    }
    
    void Renderer::Clear(const Color& color) {
        if (!m_renderer) return;
        
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(m_renderer);
    }
    
    void Renderer::Present() {
        if (!m_renderer) return;
        
        SDL_RenderPresent(m_renderer);
    }
    
    bool Renderer::SetVSync(bool enabled) {
        if (!m_renderer) {
            if (m_logger) m_logger->Warning("Cannot set VSync: Renderer not initialized");
            return false;
        }
        
        if (!SDL_SetRenderVSync(m_renderer, enabled ? 1 : 0)) {
            if (m_logger) {
                std::string error = "Failed to set VSync: ";
                error += SDL_GetError();
                m_logger->Warning(error);
            }
            return false;
        }
        
        m_vSyncEnabled = enabled;
        return true;
    }
}
