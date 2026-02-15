#pragma once

#include "Transform.h"
#include "../Core/Types.h"
#include <string>
#include <memory>

namespace Engine {

    class ILogger;

    /// <summary>
    /// Base entity class with transform and lifecycle.
    /// Provides foundation for all game objects.
    /// </summary>
    class Entity {
    public:
        Entity(const std::string& name = "", ILogger* logger = nullptr);
        virtual ~Entity();

        // Lifecycle
        virtual void Update(float deltaTime) {}
        virtual void Render() {}

        // Transform accessors
        Transform& GetTransform() { return m_transform; }
        const Transform& GetTransform() const { return m_transform; }

        void SetTransform(const Transform& transform) { m_transform = transform; }

        // Position shortcuts
        void SetPosition(int x, int y) { m_transform.SetPosition(x, y); }
        void SetPosition(const Point& pos) { m_transform.SetPosition(pos); }
        Point GetPosition() const { return m_transform.position; }

        // Rotation shortcut
        void SetRotation(float degrees) { m_transform.SetRotation(degrees); }
        float GetRotation() const { return m_transform.rotation; }

        // Scale shortcut
        void SetScale(float scale) { m_transform.SetScale(scale); }
        float GetScale() const { return m_transform.scale; }

        // Name/ID
        uint32_t GetId() const { return m_id; }
        const std::string& GetName() const { return m_name; }
        void SetName(const std::string& name) { m_name = name; }

        // Active state
        bool IsActive() const { return m_active; }
        void SetActive(bool active) { m_active = active; }

    protected:
        uint32_t m_id;
        Transform m_transform;
        std::string m_name;
        bool m_active;
        ILogger* m_logger;

    private:
        static uint32_t s_nextId;
    };

} // namespace Engine
