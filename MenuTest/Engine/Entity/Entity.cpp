#include "Entity.h"
#include "../Core/Logger/ILogger.h"

namespace Engine {

    uint32_t Entity::s_nextId = 1;

    Entity::Entity(const std::string& name, ILogger* logger)
        : m_id(s_nextId++)
        , m_transform()
        , m_name(name)
        , m_active(true)
        , m_logger(logger) {

        if (m_logger && !m_name.empty()) {
            m_logger->Debug("Entity created: " + m_name + " (ID: " + std::to_string(m_id) + ")");
        }
    }

    Entity::~Entity() {
        if (m_logger && !m_name.empty()) {
            m_logger->Debug("Entity destroyed: " + m_name + " (ID: " + std::to_string(m_id) + ")");
        }
    }

} // namespace Engine
