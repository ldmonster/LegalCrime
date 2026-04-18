#pragma once

#include "EntityManager.h"

namespace Engine {
namespace ECS {

    /// Base class for all ECS systems.
    /// Subclasses override Update() to process relevant component storages.
    class System {
    public:
        virtual ~System() = default;

        /// Called once per frame with the delta time and entity manager.
        virtual void Update(float deltaTime, EntityManager& entities) = 0;
    };

} // namespace ECS
} // namespace Engine
