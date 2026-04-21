#include "Renderer.h"
#include <SDL3/SDL.h>
#include <vector>
#include <cmath>

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

    void Renderer::SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        if (!m_renderer) return;
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
    }

    void Renderer::DrawLines(const Point* points, int count) {
        if (!m_renderer || !points || count < 2) return;

        // Convert Engine::Point to SDL_FPoint
        std::vector<SDL_FPoint> fpoints(count);
        for (int i = 0; i < count; ++i) {
            fpoints[i].x = static_cast<float>(points[i].x);
            fpoints[i].y = static_cast<float>(points[i].y);
        }
        SDL_RenderLines(m_renderer, fpoints.data(), count);
    }

    void Renderer::DrawCircle(const Point& center, int radius, int segments) {
        if (!m_renderer || radius <= 0) {
            return;
        }

        if (segments < 8) {
            segments = 8;
        }

        std::vector<Point> points(static_cast<size_t>(segments) + 1);
        for (int i = 0; i <= segments; ++i) {
            float angle = (static_cast<float>(i) * 2.0f * 3.14159f) / static_cast<float>(segments);
            points[static_cast<size_t>(i)].x = static_cast<int>(center.x + radius * std::cos(angle));
            points[static_cast<size_t>(i)].y = static_cast<int>(center.y + radius * std::sin(angle));
        }

        DrawLines(points.data(), segments + 1);
    }
}
