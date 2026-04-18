#pragma once

#include "../../../Engine/Core/Types.h"
#include "../../GameConstants.h"
#include <cstdint>

namespace Engine {
    class ILogger;
}

namespace LegalCrime {
namespace Entities {
    class Character;
}
namespace World {
    class World;

    /// Steering-based local avoidance.
    /// Each frame, applies a small repulsion force so nearby units don't overlap.
    class SteeringSystem {
    public:
        SteeringSystem(float separationRadius = Constants::Steering::DEFAULT_SEPARATION_RADIUS,
                        float separationStrength = Constants::Steering::DEFAULT_SEPARATION_STRENGTH,
                        Engine::ILogger* logger = nullptr);
        ~SteeringSystem();

        /// Apply separation steering to all units each frame.
        void Update(World* world, float deltaTime);

        void SetSeparationRadius(float radius) { m_separationRadius = radius; }
        float GetSeparationRadius() const { return m_separationRadius; }

        void SetSeparationStrength(float strength) { m_separationStrength = strength; }
        float GetSeparationStrength() const { return m_separationStrength; }

    private:
        Engine::ILogger* m_logger;
        float m_separationRadius;
        float m_separationStrength;
    };

} // namespace World
} // namespace LegalCrime
