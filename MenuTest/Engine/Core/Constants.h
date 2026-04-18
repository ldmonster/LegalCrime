#pragma once

namespace Engine {
namespace Constants {

    // Camera defaults
    namespace Camera {
        constexpr float MIN_ZOOM = 0.1f;
        constexpr float MAX_ZOOM = 5.0f;
        constexpr float DEFAULT_FOLLOW_SMOOTHNESS = 0.1f;
        constexpr float MIN_FOLLOW_SMOOTHNESS = 0.01f;
        constexpr float MAX_FOLLOW_SMOOTHNESS = 1.0f;
        constexpr float SHAKE_FREQUENCY_X = 50.0f;
        constexpr float SHAKE_FREQUENCY_Y = 45.0f;
    }

    // Pathfinding defaults
    namespace Pathfinding {
        constexpr float DIAGONAL_COST = 1.414f;
        constexpr size_t DEFAULT_POOL_CAPACITY = 4096;
    }

    // Default window dimensions
    namespace Window {
        constexpr int DEFAULT_WIDTH = 800;
        constexpr int DEFAULT_HEIGHT = 600;
    }

} // namespace Constants
} // namespace Engine
