#include "Scene.h"

namespace Engine {
    
    Scene::Scene(const std::string& name, ILogger* logger, IRenderer* renderer)
        : m_name(name)
        , m_logger(logger)
        , m_renderer(renderer)
        , m_initialized(false) {
    }
}
