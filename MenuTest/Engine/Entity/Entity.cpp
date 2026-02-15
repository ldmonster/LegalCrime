#include "Entity.h"
#include "../Core/Logger/ILogger.h"

namespace Engine {

    Entity::Entity(const std::string& name, ILogger* logger)
        : m_transform()
        , m_name(name)
        , m_active(true)
        , m_logger(logger) {
        
        if (m_logger && !m_name.empty()) {
            m_logger->Debug("Entity created: " + m_name);
        }
    }

    Entity::~Entity() {
        if (m_logger && !m_name.empty()) {
            m_logger->Debug("Entity destroyed: " + m_name);
        }
    }

} // namespace Engine
