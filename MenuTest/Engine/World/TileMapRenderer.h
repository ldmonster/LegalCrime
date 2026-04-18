#pragma once

#include "../Core/Types.h"
#include "../Graphics/Texture.h"
#include <memory>

namespace Engine {

    class IRenderer;
    class ILogger;
    class Camera2D;
    class TileMap;

    /// Handles rendering of a TileMap to screen, including texture caching.
    /// Extracted from TileMap to follow SRP — TileMap owns data, TileMapRenderer draws it.
    class TileMapRenderer {
    public:
        TileMapRenderer(ILogger* logger = nullptr);
        ~TileMapRenderer();

        /// Initialize with window dimensions for positioning.
        void Initialize(int windowWidth, int windowHeight, const TileMap& map);

        /// Render the tilemap. Uses cached texture, regenerates if needed.
        void Render(IRenderer* renderer, const TileMap& map, Camera2D* camera = nullptr);

        /// Force texture regeneration (e.g., after tile data changes).
        void Invalidate();

        /// Get the map render rect (position/size on screen).
        Rect GetMapRenderRect() const { return m_mapRenderRect; }

    private:
        void RegenerateMapTexture(IRenderer* renderer, const TileMap& map);

        ILogger* m_logger;

        std::shared_ptr<Texture> m_mapTexture;
        Rect m_mapRenderRect;
        Point m_windowCenter;
        uint16_t m_mapSizeWidth;
        uint16_t m_mapSizeHeight;
        bool m_initialized;
    };

} // namespace Engine
