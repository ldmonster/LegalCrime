#pragma once

#include "../Core/Types.h"

namespace Engine {
    class IRenderer;
    class ILogger;
    class Camera2D;
    class TileMap;
}

namespace LegalCrime {
namespace World {
    class VisionSystem;
}
}

namespace Engine {

    /// Renders a fog-of-war overlay on top of the tilemap.
    /// Unexplored tiles are fully black, explored tiles are semi-transparent.
    class FogOfWarRenderer {
    public:
        FogOfWarRenderer(ILogger* logger = nullptr);
        ~FogOfWarRenderer();

        /// Render the fog overlay based on the current vision state.
        void Render(IRenderer* renderer, const TileMap& map,
                    const LegalCrime::World::VisionSystem& vision,
                    Camera2D* camera = nullptr);

        /// Enable/disable fog rendering.
        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

    private:
        ILogger* m_logger;
        bool m_enabled;
    };

} // namespace Engine
