#include "SteeringSystem.h"
#include "../World.h"
#include "../../Entities/Character.h"
#include "../../GameConstants.h"
#include "../../../Engine/Core/Logger/ILogger.h"
#include <cmath>

namespace LegalCrime {
namespace World {

    SteeringSystem::SteeringSystem(float separationRadius, float separationStrength,
                                    Engine::ILogger* logger)
        : m_logger(logger)
        , m_separationRadius(separationRadius)
        , m_separationStrength(separationStrength) {
    }

    SteeringSystem::~SteeringSystem() = default;

    void SteeringSystem::Update(World* world, float deltaTime) {
        if (!world || deltaTime <= 0.0f) return;

        auto characters = world->GetAllCharacters();
        size_t count = characters.size();
        if (count < 2) return;

        // For each character, compute separation force from nearby characters
        for (size_t i = 0; i < count; ++i) {
            auto* a = characters[i];
            if (!a || !a->IsActive() || a->IsMoving()) continue;

            Engine::Point posA = a->GetPosition();
            float pushX = 0.0f, pushY = 0.0f;

            for (size_t j = 0; j < count; ++j) {
                if (i == j) continue;
                auto* b = characters[j];
                if (!b || !b->IsActive()) continue;

                Engine::Point posB = b->GetPosition();
                float dx = static_cast<float>(posA.x - posB.x);
                float dy = static_cast<float>(posA.y - posB.y);
                float dist = std::sqrt(dx * dx + dy * dy);

                if (dist < m_separationRadius && dist > Constants::Steering::MIN_DISTANCE) {
                    // Normalize direction and scale by inverse distance
                    float factor = (m_separationRadius - dist) / m_separationRadius;
                    pushX += (dx / dist) * factor * m_separationStrength;
                    pushY += (dy / dist) * factor * m_separationStrength;
                }
            }

            // Apply push as pixel offset (scaled by dt)
            if (std::abs(pushX) > Constants::Steering::PUSH_THRESHOLD || std::abs(pushY) > Constants::Steering::PUSH_THRESHOLD) {
                int newX = posA.x + static_cast<int>(pushX * deltaTime);
                int newY = posA.y + static_cast<int>(pushY * deltaTime);
                a->SetPosition(newX, newY);
            }
        }
    }

} // namespace World
} // namespace LegalCrime
