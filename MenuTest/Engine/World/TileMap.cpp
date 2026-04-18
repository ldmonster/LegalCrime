#include "TileMap.h"
#include "IsometricMath.h"
#include "../Renderer/IRenderer.h"
#include "../Core/Logger/ILogger.h"
#include "../Camera/Camera2D.h"
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

        // Draw tile outline (default black)
        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
        SDL_RenderLines(sdlRenderer, points, 5);

        // Fill obstacle tiles with a different color
        if (!m_walkable) {
            SDL_SetRenderDrawColor(sdlRenderer, 180, 50, 50, 255);

            for (int y = (int)points[1].y; y <= (int)points[3].y; ++y) {
                float t = (y - points[1].y) / (points[3].y - points[1].y);

                float leftX, rightX;
                if (t < 0.5f) {
                    float t2 = t * 2.0f;
                    leftX = points[1].x - (points[1].x - points[0].x) * t2;
                    rightX = points[1].x + (points[2].x - points[1].x) * t2;
                } else {
                    float t2 = (t - 0.5f) * 2.0f;
                    leftX = points[0].x + (points[3].x - points[0].x) * t2;
                    rightX = points[2].x - (points[2].x - points[3].x) * t2;
                }

                SDL_RenderLine(sdlRenderer, leftX, (float)y, rightX, (float)y);
            }

            SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
            SDL_RenderLines(sdlRenderer, points, 5);
        }

        return true;
    }
    
    // TileMap implementation
    TileMap::TileMap(uint16_t width, uint16_t height, ILogger* logger)
        : m_logger(logger)
        , m_mapWidth(width)
        , m_mapHeight(height)
        , m_tileWidth(DEFAULT_TILE_WIDTH)
        , m_tileHeight(DEFAULT_TILE_HEIGHT)
        , m_mapSizeWidth(0)
        , m_mapSizeHeight(0)
        , m_windowCenter(0, 0)
        , m_offsetX(0)
        , m_offsetY(0)
        , m_initialized(false) {
        
        IsometricMath::CalculateMapSize(width, height, m_tileWidth, m_tileHeight,
                                        m_mapSizeWidth, m_mapSizeHeight);

        m_tiles.resize(height);
        for (uint16_t i = 0; i < height; i++) {
            m_tiles[i].resize(width);
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
        float diamondWidth = (m_mapWidth - 1 + m_mapHeight - 1) * m_tileWidth;
        float diamondHeight = (m_mapWidth - 1 + m_mapHeight - 1) * m_tileHeight;

        int boundWidth = static_cast<int>(diamondWidth);
        int boundHeight = static_cast<int>(diamondHeight);

        int boundX = -boundWidth / 2 + m_windowCenter.x;
        int boundY = -boundHeight / 2 + m_windowCenter.y;

        return Rect(boundX, boundY, boundWidth, boundHeight);
    }

    void TileMap::ClampOffsetToBounds() {
        IsometricMath::ClampToDiamondBounds(
            m_offsetX, m_offsetY,
            m_mapWidth, m_mapHeight,
            m_tileWidth, m_tileHeight);
    }

    Point TileMap::TileToScreen(uint16_t row, uint16_t col, Camera2D* camera) const {
        if (!m_initialized) {
            return Point(0, 0);
        }

        int originX = 0;
        int originY = m_mapWidth * m_tileHeight;

        Point tilePos = IsometricMath::TileToLocal(
            row, col, originX, originY, m_tileWidth, m_tileHeight);

        if (camera) {
            int worldX = tilePos.x - m_mapSizeWidth / 2;
            int worldY = tilePos.y - m_mapSizeHeight / 2;
            return camera->WorldToScreen(Point(worldX, worldY));
        } else {
            int screenX = tilePos.x + m_mapRenderRect.x + m_offsetX;
            int screenY = tilePos.y + m_mapRenderRect.y + m_offsetY;
            return Point(screenX, screenY);
        }
    }

    Point TileMap::TileToScreenCenter(uint16_t row, uint16_t col, Camera2D* camera) const {
        Point topLeft = TileToScreen(row, col, camera);
        topLeft.x += m_tileWidth;
        return topLeft;
    }

    bool TileMap::ScreenToTile(int screenX, int screenY, uint16_t& outRow, uint16_t& outCol, Camera2D* camera) const {
        if (!m_initialized) {
            return false;
        }

        int mapX, mapY;

        if (camera) {
            Point worldPos = camera->ScreenToWorld(Point(screenX, screenY));
            mapX = worldPos.x + m_mapSizeWidth / 2;
            mapY = worldPos.y + m_mapSizeHeight / 2;
        } else {
            mapX = screenX - m_mapRenderRect.x - m_offsetX;
            mapY = screenY - m_mapRenderRect.y - m_offsetY;
        }

        int originX = 0;
        int originY = m_mapWidth * m_tileHeight;

        return IsometricMath::LocalToTile(
            mapX, mapY, originX, originY,
            m_tileWidth, m_tileHeight,
            m_mapWidth, m_mapHeight,
            outRow, outCol);
    }

    bool TileMap::ScreenToTile(int screenX, int screenY, TilePosition& outPos, Camera2D* camera) const {
        return ScreenToTile(screenX, screenY, outPos.row, outPos.col, camera);
    }
}

