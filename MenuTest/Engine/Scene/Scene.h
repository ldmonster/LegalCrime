#pragma once

#include "IScene.h"
#include "../Core/Logger/ILogger.h"
#include "../Renderer/IRenderer.h"

namespace Engine {
    
    // Base scene class with common functionality
    class Scene : public IScene {
    public:
        Scene(const std::string& name, ILogger* logger, IRenderer* renderer);
        virtual ~Scene() = default;
        
        const std::string& GetName() const override { return m_name; }
        bool IsInitialized() const override { return m_initialized; }
        
    protected:
        std::string m_name;
        ILogger* m_logger;
        IRenderer* m_renderer;
        bool m_initialized;
    };
}
