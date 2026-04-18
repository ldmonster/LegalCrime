#pragma once

#include "../Core/Types.h"
#include "../Graphics/Texture.h"
#include <vector>
#include <memory>

namespace Engine {

    class IRenderer;
    class ILogger;
    class Camera2D;
    class TileMap;
    class Entity;

    /// Minimap widget rendered in a corner of the screen.
    /// Shows a downscaled tilemap, unit dots, and the camera viewport.
    /// Click on the minimap to move the camera.
    class Minimap {
    public:
        Minimap(int width, int height, ILogger* logger = nullptr);
        ~Minimap();

        /// Set screen position of the minimap (top-left corner).
        void SetPosition(int x, int y) { m_screenX = x; m_screenY = y; }

        /// Update the minimap texture from tilemap data. Call when map changes.
        void UpdateFromTileMap(IRenderer* renderer, const TileMap& map);

        /// Render the minimap with unit dots and viewport rectangle.
        void Render(IRenderer* renderer, const TileMap& map,
                    const std::vector<Entity*>& entities,
                    Camera2D* camera = nullptr);

        /// Check if a screen position is inside the minimap.
        bool ContainsPoint(int screenX, int screenY) const;

        /// Convert a click on the minimap to a world position (for camera movement).
        Point ScreenToWorld(int screenX, int screenY, const TileMap& map) const;

        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

        void SetUnitDotColor(const Color& color) { m_unitDotColor = color; }

    private:
        ILogger* m_logger;
        int m_width;
        int m_height;
        int m_screenX;
        int m_screenY;
        bool m_dirty;

        Color m_unitDotColor;
        std::shared_ptr<Texture> m_mapTexture;
    };

} // namespace Engine
