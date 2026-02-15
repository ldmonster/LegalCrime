#include "TileMap.h"
#include "../Renderer/IRenderer.h"
#include "../Core/Logger/ILogger.h"
#include <SDL3/SDL.h>
#include <cmath>

namespace Engine {
    
    // Tile rendering implementation
    bool Tile::Render(IRenderer* renderer, const Point& topLeft, int tileWidth, int tileHeight) const {
        if (!renderer) return false;
        
        SDL_Renderer* sdlRenderer = renderer->GetNativeRenderer();
        
        // Calculate isometric diamond points
        int xMult = tileWidth;
        int yMult = tileHeight;
        
        SDL_FPoint points[5];
        points[0] = SDL_FPoint{ (float)topLeft.x, (float)topLeft.y };
        points[1] = SDL_FPoint{ (float)(topLeft.x + xMult), (float)(topLeft.y - yMult) };
        points[2] = SDL_FPoint{ (float)(topLeft.x + 2 * xMult), (float)topLeft.y };
        points[3] = SDL_FPoint{ (float)(topLeft.x + xMult), (float)(topLeft.y + yMult) };
        points[4] = SDL_FPoint{ (float)topLeft.x, (float)topLeft.y };
        
        return SDL_RenderLines(sdlRenderer, points, 5);
    }
    
    // TileMap implementation
    TileMap::TileMap(uint16_t width, uint16_t height, ILogger* logger)
        : m_logger(logger)
        , m_mapWidth(width)
        , m_mapHeight(height)
        , m_tileWidth(DEFAULT_TILE_WIDTH)
        , m_tileHeight(DEFAULT_TILE_HEIGHT)  // Half of tileWidth for isometric
        , m_mapSizeWidth((width + height) * DEFAULT_TILE_WIDTH)
        , m_mapSizeHeight((width + height) * DEFAULT_TILE_HEIGHT)
        , m_mapTexture(nullptr)
        , m_windowCenter(0, 0)
        , m_offsetX(0)
        , m_offsetY(0)
        , m_isDragging(false)
        , m_dragStart(0, 0)
        , m_initialOffset(0, 0)
        , m_initialized(false) {
        
        // Initialize tile grid with proper vector sizing
        m_tiles.resize(height);
        for (uint16_t i = 0; i < height; i++) {
            m_tiles[i].resize(width);
            // Initialize all tiles with ID 0
            for (uint16_t j = 0; j < width; j++) {
                m_tiles[i][j].SetId(0);
            }
        }
        
        if (m_logger) {
            m_logger->Debug("TileMap created: " + std::to_string(width) + "x" + std::to_string(height));
        }
    }
    
    TileMap::~TileMap() {
        if (m_logger) {
            m_logger->Debug("TileMap destroyed");
        }
    }
    
    Result<void> TileMap::Initialize(int windowWidth, int windowHeight) {
        if (m_initialized) {
            return Result<void>::Failure("TileMap already initialized");
        }
        
        m_windowCenter.x = windowWidth / 2;
        m_windowCenter.y = windowHeight / 2;
        
        // Calculate render rectangle (centered on window)
        m_mapRenderRect.x = -m_mapSizeWidth / 2 + m_windowCenter.x;
        m_mapRenderRect.y = -m_mapSizeHeight / 2 + m_windowCenter.y;
        m_mapRenderRect.w = m_mapSizeWidth;
        m_mapRenderRect.h = m_mapSizeHeight;
        
        m_initialized = true;
        
        if (m_logger) {
            m_logger->Info("TileMap initialized at window size: " + 
                          std::to_string(windowWidth) + "x" + std::to_string(windowHeight));
        }
        
        return Result<void>::Success();
    }
    
    void TileMap::Render(IRenderer* renderer) {
        if (!m_initialized || !renderer) {
            return;
        }
        
        // Regenerate texture if needed
        if (!m_mapTexture) {
            if (m_logger) {
                m_logger->Debug("Generating tilemap texture...");
            }
            RegenerateMapTexture(renderer);
        }
        
        // Render the pre-generated map texture
        if (m_mapTexture) {
            // Apply camera offset
            Rect renderRect = m_mapRenderRect;
            renderRect.x += m_offsetX;
            renderRect.y += m_offsetY;
            
            Rect sourceRect(0, 0, m_mapSizeWidth, m_mapSizeHeight);
            m_mapTexture->Render(renderer, renderRect, &sourceRect);
        }
    }
    
    void TileMap::RegenerateMapTexture(IRenderer* renderer) {
        if (!renderer) return;
        
        SDL_Renderer* sdlRenderer = renderer->GetNativeRenderer();
        
        // Create render target texture
        SDL_Texture* targetTexture = SDL_CreateTexture(
            sdlRenderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            m_mapSizeWidth,
            m_mapSizeHeight
        );
        
        if (!targetTexture) {
            if (m_logger) {
                m_logger->Error("Failed to create tilemap texture: " + std::string(SDL_GetError()));
            }
            return;
        }
        
        // Render to texture
        SDL_SetRenderTarget(sdlRenderer, targetTexture);
        SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 255);
        SDL_RenderClear(sdlRenderer);
        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
        
        // Render all tiles
        uint16_t firstX = 0;
        // Position firstY so the topmost point of the topmost tile diamond is at Y=0
        // The topmost tile (j=width-1, i=0) has tilePos.y = firstY - (width-1)*tileHeight
        // The TOP of the diamond is at tilePos.y - tileHeight
        // We want: firstY - (width-1)*tileHeight - tileHeight = 0
        // Therefore: firstY = width * tileHeight
        uint16_t firstY = m_mapWidth * m_tileHeight;
        
        for (uint16_t i = 0; i < m_mapHeight; i++) {
            for (uint16_t j = 0; j < m_mapWidth; j++) {
                Point tilePos(
                    firstX + (j + i) * m_tileWidth,
                    firstY - (j - i) * m_tileHeight
                );
                
                m_tiles[i][j].Render(renderer, tilePos, m_tileWidth, m_tileHeight);
            }
        }
        
        // Draw border
        SDL_FPoint borderPoints[5];
        borderPoints[0] = SDL_FPoint{ 0, 0 };
        borderPoints[1] = SDL_FPoint{ (float)(m_mapSizeWidth - 1), 0 };
        borderPoints[2] = SDL_FPoint{ (float)(m_mapSizeWidth - 1), (float)(m_mapSizeHeight - 1) };
        borderPoints[3] = SDL_FPoint{ 0, (float)(m_mapSizeHeight - 1) };
        borderPoints[4] = SDL_FPoint{ 0, 0 };
        SDL_RenderLines(sdlRenderer, borderPoints, 5);
        
        // Reset render target
        SDL_SetRenderTarget(sdlRenderer, nullptr);

        // Wrap in Engine::Texture
        m_mapTexture = Texture::CreateFromSDL(targetTexture, m_mapSizeWidth, m_mapSizeHeight);

        if (m_logger) {
            m_logger->Debug("Tilemap texture generated successfully");
        }
    }
    
    void TileMap::HandleEvent(const SDL_Event& event) {
        if (!m_initialized) return;

        // Mouse dragging
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
            m_isDragging = true;
            m_dragStart = Point((int)event.button.x, (int)event.button.y);
            m_initialOffset = Point(m_offsetX, m_offsetY);
        }
        else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT) {
            m_isDragging = false;
        }
        else if (event.type == SDL_EVENT_MOUSE_MOTION && m_isDragging) {
            int deltaX = (int)event.motion.x - m_dragStart.x;
            int deltaY = (int)event.motion.y - m_dragStart.y;

            m_offsetX = m_initialOffset.x + deltaX;
            m_offsetY = m_initialOffset.y + deltaY;
            ClampOffsetToBounds();
        }
        // Keyboard arrow keys
        else if (event.type == SDL_EVENT_KEY_DOWN) {
            const int panSpeed = 20; // Pixels to move per key press

            switch (event.key.key) {
                case SDLK_UP:
                    m_offsetY += panSpeed;
                    break;
                case SDLK_DOWN:
                    m_offsetY -= panSpeed;
                    break;
                case SDLK_LEFT:
                    m_offsetX += panSpeed;
                    break;
                case SDLK_RIGHT:
                    m_offsetX -= panSpeed;
                    break;
            }
            ClampOffsetToBounds();
        }
    }
    
    Tile* TileMap::GetTile(uint16_t row, uint16_t col) {
        if (row >= m_mapHeight || col >= m_mapWidth) {
            return nullptr;
        }
        return &m_tiles[row][col];
    }
    
    const Tile* TileMap::GetTile(uint16_t row, uint16_t col) const {
        if (row >= m_mapHeight || col >= m_mapWidth) {
            return nullptr;
        }
        return &m_tiles[row][col];
    }

    void TileMap::SetOffset(int x, int y) {
        m_offsetX = x;
        m_offsetY = y;
        ClampOffsetToBounds();
    }

    Rect TileMap::GetBounds() const {
        // Bounds based on actual diamond corner positions
        float diamondWidth = (m_mapWidth - 1 + m_mapHeight - 1) * m_tileWidth;
        float diamondHeight = (m_mapWidth - 1 + m_mapHeight - 1) * m_tileHeight;

        int boundWidth = static_cast<int>(diamondWidth);
        int boundHeight = static_cast<int>(diamondHeight);

        // Center the bounds around the window center
        int boundX = -boundWidth / 2 + m_windowCenter.x;
        int boundY = -boundHeight / 2 + m_windowCenter.y;

        return Rect(boundX, boundY, boundWidth, boundHeight);
    }

    void TileMap::ClampOffsetToBounds() {
        // Rhombus (diamond) bounds based on actual tile corner positions
        // The isometric diamond has 4 vertices:
        // - Top: tile (0, width-1)
        // - Right: tile (0, 0)
        // - Bottom: tile (height-1, 0)
        // - Left: tile (height-1, width-1)

        // Calculate the actual diamond dimensions from corner tile positions
        // Horizontal extent: from right corner to left corner
        // Right is at x=0, Left is at x=(width-1+height-1)*tileWidth
        float diamondWidth = (m_mapWidth - 1 + m_mapHeight - 1) * m_tileWidth;

        // Vertical extent: from top corner to bottom corner
        // Top is at y=-(width-1)*tileHeight, Bottom is at y=+(height-1)*tileHeight
        float diamondHeight = (m_mapWidth - 1 + m_mapHeight - 1) * m_tileHeight;

        // Automatically adjust bounds multiplier based on map aspect ratio
        // Square maps (1:1) get tighter bounds (1.0x = 50% visible at edges)
        // Rectangular maps get looser bounds (up to 2.0x = 25% visible at edges)
        float aspectRatio = static_cast<float>(m_mapWidth > m_mapHeight ? 
            m_mapWidth : m_mapHeight) / static_cast<float>(m_mapWidth < m_mapHeight ? 
            m_mapWidth : m_mapHeight);

        // Interpolate multiplier: 1.0x for square (ratio=1), 2.0x for very rectangular (ratio=10+)
        float boundMultiplier = 1.0f + (std::min(aspectRatio, 10.0f) - 1.0f) / 9.0f;

        float boundHalfWidth = (diamondWidth / 2.0f) * boundMultiplier;
        float boundHalfHeight = (diamondHeight / 2.0f) * boundMultiplier;

        // For a diamond/rhombus, the edge equation is: |x|/w + |y|/h <= 1
        // This works for all 4 edges (top, right, bottom, left)

        float normalizedX = std::abs(static_cast<float>(m_offsetX)) / boundHalfWidth;
        float normalizedY = std::abs(static_cast<float>(m_offsetY)) / boundHalfHeight;
        float distance = normalizedX + normalizedY;

        // If outside the diamond bounds, scale back to the edge
        if (distance > 1.0f) {
            // Scale the offset to be exactly on the diamond edge
            float scale = 1.0f / distance;
            m_offsetX = static_cast<int>(m_offsetX * scale);
            m_offsetY = static_cast<int>(m_offsetY * scale);
        }
    }

    Point TileMap::TileToScreen(uint16_t row, uint16_t col) const {
        if (!m_initialized) {
            return Point(0, 0);
        }

        // Calculate tile position in map texture space
        int firstX = 0;
        int firstY = m_mapWidth * m_tileHeight;

        int tileX = firstX + (col + row) * m_tileWidth;
        int tileY = firstY - (col - row) * m_tileHeight;

        // Convert to screen space with map offset and camera offset
        int screenX = tileX + m_mapRenderRect.x + m_offsetX;
        int screenY = tileY + m_mapRenderRect.y + m_offsetY;

        return Point(screenX, screenY);
    }

    Point TileMap::TileToScreenCenter(uint16_t row, uint16_t col) const {
        Point topLeft = TileToScreen(row, col);

        // Center of isometric diamond is at (tileWidth, 0) from top-left point
        topLeft.x += m_tileWidth;

        return topLeft;
    }
}
