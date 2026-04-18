#pragma once

namespace LegalCrime {
namespace Constants {

    // Movement
    namespace Movement {
        constexpr float DEFAULT_MOVE_DURATION = 0.3f;
    }

    // Steering / Collision avoidance
    namespace Steering {
        constexpr float DEFAULT_SEPARATION_RADIUS = 40.0f;
        constexpr float DEFAULT_SEPARATION_STRENGTH = 100.0f;
        constexpr float MIN_DISTANCE = 0.1f;
        constexpr float PUSH_THRESHOLD = 0.01f;
    }

    // Main menu UI layout
    namespace MainMenu {
        constexpr int BUTTON_WIDTH = 566;
        constexpr int BUTTON_HEIGHT = 84;
        constexpr int BUTTON_HIT_Y_OFFSET = 504;
        constexpr int BUTTON_RENDER_X = 257;
        constexpr int BUTTON_RENDER_Y = 152;
        constexpr int BUTTON_RENDER_WIDTH = 283;
        constexpr int BUTTON_RENDER_HEIGHT = 42;
        constexpr int BUTTON_RENDER_Y_OFFSET = 26;
        constexpr int EXIT_BUTTON_Y_ADJUST = 2;
    }

} // namespace Constants
} // namespace LegalCrime
