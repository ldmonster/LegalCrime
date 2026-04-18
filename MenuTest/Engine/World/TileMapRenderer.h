#pragma once

#include "../Core/Types.h"
#include "../Graphics/Texture.h"
#include <memory>
#include <vector>

namespace Engine {

    class IRenderer;
    class ILogger;
    class Camera2D;
    class TileMap;

    /// Handles rendering of a TileMap to screen using chunked texture caching.
    /// The map is divided into CHUNK_SIZE x CHUNK_SIZE tile chunks.
    /// Only dirty chunks are re-rendered; only visible chunks are drawn.
    class TileMapRenderer {
    public:
        static constexpr uint16_t CHUNK_SIZE = 16; // tiles per chunk edge

        TileMapRenderer(ILogger* logger = nullptr);
        ~TileMapRenderer();

        /// Initialize with window dimensions for positioning.
        void Initialize(int windowWidth, int windowHeight, const TileMap& map);

        /// Render the tilemap. Draws only chunks visible in the viewport.
        void Render(IRenderer* renderer, const TileMap& map, Camera2D* camera = nullptr);

        /// Mark a specific chunk as dirty (e.g., after tile data changes).
        void InvalidateChunk(uint16_t chunkRow, uint16_t chunkCol);

        /// Force all chunks to be regenerated.
        void Invalidate();

        /// Get the map render rect (position/size on screen).
        Rect GetMapRenderRect() const { return m_mapRenderRect; }

    private:
        struct Chunk {
            std::shared_ptr<Texture> texture;
            bool dirty = true;
            Rect localRect; // position within the full map texture space
        };

        void RegenerateChunk(IRenderer* renderer, const TileMap& map, uint16_t chunkRow, uint16_t chunkCol);
        bool IsChunkVisible(const Chunk& chunk, const Rect& viewport) const;

        ILogger* m_logger;

        std::vector<Chunk> m_chunks;
        uint16_t m_chunksPerRow;
        uint16_t m_chunksPerCol;

        Rect m_mapRenderRect;
        Point m_windowCenter;
        uint16_t m_mapSizeWidth;
        uint16_t m_mapSizeHeight;
        bool m_initialized;
    };

} // namespace Engine
