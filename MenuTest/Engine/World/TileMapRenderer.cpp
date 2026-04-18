#include "TileMapRenderer.h"
#include "TileMap.h"
#include "IsometricMath.h"
#include "../Renderer/IRenderer.h"
#include "../Camera/Camera2D.h"
#include "../Core/Logger/ILogger.h"
#include <SDL3/SDL.h>
#include <algorithm>

namespace Engine {

    TileMapRenderer::TileMapRenderer(ILogger* logger)
        : m_logger(logger)
        , m_chunksPerRow(0)
        , m_chunksPerCol(0)
        , m_windowCenter(0, 0)
        , m_mapSizeWidth(0)
        , m_mapSizeHeight(0)
        , m_initialized(false) {
    }

    TileMapRenderer::~TileMapRenderer() = default;

    void TileMapRenderer::Initialize(int windowWidth, int windowHeight, const TileMap& map) {
        m_windowCenter.x = windowWidth / 2;
        m_windowCenter.y = windowHeight / 2;

        IsometricMath::CalculateMapSize(
            map.GetWidth(), map.GetHeight(),
            map.GetTileWidth(), map.GetTileHeight(),
            m_mapSizeWidth, m_mapSizeHeight);

        m_mapRenderRect.x = -m_mapSizeWidth / 2 + m_windowCenter.x;
        m_mapRenderRect.y = -m_mapSizeHeight / 2 + m_windowCenter.y;
        m_mapRenderRect.w = m_mapSizeWidth;
        m_mapRenderRect.h = m_mapSizeHeight;

        // Compute chunk grid dimensions
        m_chunksPerRow = (map.GetWidth() + CHUNK_SIZE - 1) / CHUNK_SIZE;
        m_chunksPerCol = (map.GetHeight() + CHUNK_SIZE - 1) / CHUNK_SIZE;

        m_chunks.resize(static_cast<size_t>(m_chunksPerRow) * m_chunksPerCol);

        // Pre-calculate each chunk's local rect (bounding box within full map space)
        for (uint16_t cr = 0; cr < m_chunksPerCol; ++cr) {
            for (uint16_t cc = 0; cc < m_chunksPerRow; ++cc) {
                uint16_t tileRowStart = cr * CHUNK_SIZE;
                uint16_t tileColStart = cc * CHUNK_SIZE;
                uint16_t tileRowEnd = std::min<uint16_t>(tileRowStart + CHUNK_SIZE, map.GetHeight());
                uint16_t tileColEnd = std::min<uint16_t>(tileColStart + CHUNK_SIZE, map.GetWidth());

                uint16_t originX = 0;
                uint16_t originY = map.GetWidth() * map.GetTileHeight();

                // Compute bounding box of all tiles in this chunk
                int minX = INT32_MAX, minY = INT32_MAX;
                int maxX = INT32_MIN, maxY = INT32_MIN;

                for (uint16_t r = tileRowStart; r < tileRowEnd; ++r) {
                    for (uint16_t c = tileColStart; c < tileColEnd; ++c) {
                        Point p = IsometricMath::TileToLocal(r, c, originX, originY,
                                                             map.GetTileWidth(), map.GetTileHeight());
                        minX = std::min(minX, p.x);
                        minY = std::min(minY, p.y - map.GetTileHeight());
                        maxX = std::max(maxX, p.x + 2 * map.GetTileWidth());
                        maxY = std::max(maxY, p.y + map.GetTileHeight());
                    }
                }

                auto& chunk = m_chunks[static_cast<size_t>(cr) * m_chunksPerRow + cc];
                chunk.localRect = Rect(minX, minY, maxX - minX, maxY - minY);
                chunk.dirty = true;
            }
        }

        m_initialized = true;

        if (m_logger) {
            m_logger->Debug("TileMapRenderer initialized (" +
                           std::to_string(m_chunksPerRow) + "x" +
                           std::to_string(m_chunksPerCol) + " chunks)");
        }
    }

    void TileMapRenderer::Render(IRenderer* renderer, const TileMap& map, Camera2D* camera) {
        if (!m_initialized || !renderer) return;

        // Determine the viewport rect in map-local space
        Rect viewport;
        if (camera) {
            // Screen (0,0) → world, then offset into map-local space
            Point topLeft = camera->ScreenToWorld(Point(0, 0));
            // Approximate screen size: use map render rect as reference
            int screenW = m_windowCenter.x * 2;
            int screenH = m_windowCenter.y * 2;
            Point bottomRight = camera->ScreenToWorld(Point(screenW, screenH));
            viewport.x = topLeft.x + m_mapSizeWidth / 2;
            viewport.y = topLeft.y + m_mapSizeHeight / 2;
            viewport.w = bottomRight.x - topLeft.x;
            viewport.h = bottomRight.y - topLeft.y;
        } else {
            Point offset = map.GetOffset();
            viewport.x = -m_mapRenderRect.x - offset.x;
            viewport.y = -m_mapRenderRect.y - offset.y;
            viewport.w = m_windowCenter.x * 2;
            viewport.h = m_windowCenter.y * 2;
        }

        for (uint16_t cr = 0; cr < m_chunksPerCol; ++cr) {
            for (uint16_t cc = 0; cc < m_chunksPerRow; ++cc) {
                size_t idx = static_cast<size_t>(cr) * m_chunksPerRow + cc;
                Chunk& chunk = m_chunks[idx];

                // Viewport culling
                if (!IsChunkVisible(chunk, viewport)) continue;

                // Regenerate if dirty
                if (chunk.dirty || !chunk.texture) {
                    RegenerateChunk(renderer, map, cr, cc);
                }

                if (!chunk.texture) continue;

                // Calculate screen position for this chunk
                Rect destRect;
                if (camera) {
                    int worldX = chunk.localRect.x - m_mapSizeWidth / 2;
                    int worldY = chunk.localRect.y - m_mapSizeHeight / 2;
                    Point screenPos = camera->WorldToScreen(Point(worldX, worldY));
                    destRect.x = screenPos.x;
                    destRect.y = screenPos.y;
                    // Scale chunk size by camera zoom
                    float zoom = camera->GetZoom();
                    destRect.w = static_cast<int>(chunk.localRect.w * zoom);
                    destRect.h = static_cast<int>(chunk.localRect.h * zoom);
                } else {
                    Point offset = map.GetOffset();
                    destRect.x = chunk.localRect.x + m_mapRenderRect.x + offset.x;
                    destRect.y = chunk.localRect.y + m_mapRenderRect.y + offset.y;
                    destRect.w = chunk.localRect.w;
                    destRect.h = chunk.localRect.h;
                }

                Rect srcRect(0, 0, chunk.localRect.w, chunk.localRect.h);
                chunk.texture->Render(renderer, destRect, &srcRect);
            }
        }
    }

    void TileMapRenderer::InvalidateChunk(uint16_t chunkRow, uint16_t chunkCol) {
        size_t idx = static_cast<size_t>(chunkRow) * m_chunksPerRow + chunkCol;
        if (idx < m_chunks.size()) {
            m_chunks[idx].dirty = true;
        }
    }

    void TileMapRenderer::Invalidate() {
        for (auto& chunk : m_chunks) {
            chunk.dirty = true;
        }
    }

    void TileMapRenderer::RegenerateChunk(IRenderer* renderer, const TileMap& map,
                                           uint16_t chunkRow, uint16_t chunkCol) {
        size_t idx = static_cast<size_t>(chunkRow) * m_chunksPerRow + chunkCol;
        Chunk& chunk = m_chunks[idx];

        SDL_Renderer* sdlRenderer = renderer->GetNativeRenderer();
        if (!sdlRenderer) return;

        int texW = chunk.localRect.w;
        int texH = chunk.localRect.h;
        if (texW <= 0 || texH <= 0) return;

        SDL_Texture* targetTexture = SDL_CreateTexture(
            sdlRenderer, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET, texW, texH);

        if (!targetTexture) {
            if (m_logger) {
                m_logger->Error("Failed to create chunk texture: " + std::string(SDL_GetError()));
            }
            return;
        }

        SDL_SetTextureBlendMode(targetTexture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(sdlRenderer, targetTexture);
        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 0);
        SDL_RenderClear(sdlRenderer);

        uint16_t originX = 0;
        uint16_t originY = map.GetWidth() * map.GetTileHeight();

        uint16_t tileRowStart = chunkRow * CHUNK_SIZE;
        uint16_t tileColStart = chunkCol * CHUNK_SIZE;
        uint16_t tileRowEnd = std::min<uint16_t>(tileRowStart + CHUNK_SIZE, map.GetHeight());
        uint16_t tileColEnd = std::min<uint16_t>(tileColStart + CHUNK_SIZE, map.GetWidth());

        for (uint16_t r = tileRowStart; r < tileRowEnd; ++r) {
            for (uint16_t c = tileColStart; c < tileColEnd; ++c) {
                Point tilePos = IsometricMath::TileToLocal(r, c, originX, originY,
                                                           map.GetTileWidth(), map.GetTileHeight());
                // Offset into chunk-local coordinates
                Point localPos(tilePos.x - chunk.localRect.x, tilePos.y - chunk.localRect.y);

                const Tile* tile = map.GetTile(r, c);
                if (tile) {
                    tile->Render(renderer, localPos, map.GetTileWidth(), map.GetTileHeight());
                }
            }
        }

        SDL_SetRenderTarget(sdlRenderer, nullptr);

        chunk.texture = Texture::CreateFromSDL(targetTexture, texW, texH);
        chunk.dirty = false;
    }

    bool TileMapRenderer::IsChunkVisible(const Chunk& chunk, const Rect& viewport) const {
        // Simple AABB overlap test
        return !(chunk.localRect.x + chunk.localRect.w < viewport.x ||
                 chunk.localRect.x > viewport.x + viewport.w ||
                 chunk.localRect.y + chunk.localRect.h < viewport.y ||
                 chunk.localRect.y > viewport.y + viewport.h);
    }

} // namespace Engine
