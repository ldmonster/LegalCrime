#pragma once

#include "../Core/Types.h"
#include "../Graphics/Texture.h"
#include <SDL3/SDL.h>
#include <vector>
#include <memory>
#include <string>

namespace Engine {
    
    class IRenderer;
    class ILogger;
    
    // Represents a single tile in the tilemap
    class Tile {
    public:
        Tile() : m_id(0) {}
        explicit Tile(uint16_t id) : m_id(id) {}
        
        void SetId(uint16_t id) { m_id = id; }
        uint16_t GetId() const { return m_id; }
        
        // Render this tile as an isometric diamond
        bool Render(IRenderer* renderer, const Point& topLeft, int tileWidth, int tileHeight) const;
        
    private:
        uint16_t m_id;
    };
    
    // Modern tilemap class with proper resource management
    class TileMap {
    public:
        // Default isometric tile dimensions
        static constexpr uint16_t DEFAULT_TILE_WIDTH = 50;
        static constexpr uint16_t DEFAULT_TILE_HEIGHT = 25;  // Half of width for isometric

        TileMap(uint16_t width, uint16_t height, ILogger* logger = nullptr);
        ~TileMap();
        
        // Initialization
        Result<void> Initialize(int windowWidth, int windowHeight);
        bool IsInitialized() const { return m_initialized; }
        
        // Rendering
        void Render(IRenderer* renderer);
        
        // Event handling
        void HandleEvent(const SDL_Event& event);
        
        // Tile access
        Tile* GetTile(uint16_t row, uint16_t col);
        const Tile* GetTile(uint16_t row, uint16_t col) const;
        
        // Map properties
        uint16_t GetWidth() const { return m_mapWidth; }
        uint16_t GetHeight() const { return m_mapHeight; }
        uint16_t GetTileWidth() const { return m_tileWidth; }
        uint16_t GetTileHeight() const { return m_tileHeight; }
        
        // Camera/viewport offset
        void SetOffset(int x, int y);
        Point GetOffset() const { return Point(m_offsetX, m_offsetY); }

        // Bounds (30% additional space beyond map size)
        Rect GetBounds() const;

        // Coordinate conversion
        Point TileToScreen(uint16_t row, uint16_t col) const;
        Point TileToScreenCenter(uint16_t row, uint16_t col) const;
        bool ScreenToTile(int screenX, int screenY, uint16_t& outRow, uint16_t& outCol) const;
        
    private:
        void RenderMapTexture(IRenderer* renderer);
        void RegenerateMapTexture(IRenderer* renderer);
        void ClampOffsetToBounds();
        
        ILogger* m_logger;
        
        // Tile grid - modern vector-based storage
        std::vector<std::vector<Tile>> m_tiles;
        
        // Map dimensions
        uint16_t m_mapWidth;
        uint16_t m_mapHeight;
        
        // Tile dimensions (isometric)
        uint16_t m_tileWidth;
        uint16_t m_tileHeight;
        
        // Calculated map size in pixels
        uint16_t m_mapSizeWidth;
        uint16_t m_mapSizeHeight;
        
        // Pre-rendered map texture for performance
        std::shared_ptr<Texture> m_mapTexture;
        Rect m_mapRenderRect;
        
        // Window center for positioning
        Point m_windowCenter;
        
        // Camera offset for panning
        int m_offsetX;
        int m_offsetY;
        
        // Mouse interaction state
        bool m_isDragging;
        Point m_dragStart;
        Point m_initialOffset;
        
        bool m_initialized;
    };
}
