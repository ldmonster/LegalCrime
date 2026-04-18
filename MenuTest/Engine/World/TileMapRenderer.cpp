#include "TileMapRenderer.h"
#include "TileMap.h"
#include "IsometricMath.h"
#include "../Renderer/IRenderer.h"
#include "../Camera/Camera2D.h"
#include "../Core/Logger/ILogger.h"
#include <SDL3/SDL.h>

namespace Engine {

    TileMapRenderer::TileMapRenderer(ILogger* logger)
        : m_logger(logger)
        , m_mapTexture(nullptr)
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

        m_initialized = true;

        if (m_logger) {
            m_logger->Debug("TileMapRenderer initialized");
        }
    }

    void TileMapRenderer::Render(IRenderer* renderer, const TileMap& map, Camera2D* camera) {
        if (!m_initialized || !renderer) return;

        if (!m_mapTexture) {
            RegenerateMapTexture(renderer, map);
        }

        if (!m_mapTexture) return;

        Rect renderRect = m_mapRenderRect;

        if (camera) {
            Point worldPos(0, 0);
            Point screenPos = camera->WorldToScreen(worldPos);
            renderRect.x = screenPos.x - m_mapSizeWidth / 2;
            renderRect.y = screenPos.y - m_mapSizeHeight / 2;
        } else {
            Point offset = map.GetOffset();
            renderRect.x += offset.x;
            renderRect.y += offset.y;
        }

        Rect sourceRect(0, 0, m_mapSizeWidth, m_mapSizeHeight);
        m_mapTexture->Render(renderer, renderRect, &sourceRect);
    }

    void TileMapRenderer::Invalidate() {
        m_mapTexture.reset();
    }

    void TileMapRenderer::RegenerateMapTexture(IRenderer* renderer, const TileMap& map) {
        if (!renderer) return;

        SDL_Renderer* sdlRenderer = renderer->GetNativeRenderer();

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

        SDL_SetRenderTarget(sdlRenderer, targetTexture);
        SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 255);
        SDL_RenderClear(sdlRenderer);
        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);

        uint16_t originX = 0;
        uint16_t originY = map.GetWidth() * map.GetTileHeight();

        for (uint16_t i = 0; i < map.GetHeight(); i++) {
            for (uint16_t j = 0; j < map.GetWidth(); j++) {
                Point tilePos = IsometricMath::TileToLocal(
                    i, j, originX, originY,
                    map.GetTileWidth(), map.GetTileHeight());

                const Tile* tile = map.GetTile(i, j);
                if (tile) {
                    tile->Render(renderer, tilePos, map.GetTileWidth(), map.GetTileHeight());
                }
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

        SDL_SetRenderTarget(sdlRenderer, nullptr);

        m_mapTexture = Texture::CreateFromSDL(targetTexture, m_mapSizeWidth, m_mapSizeHeight);

        if (m_logger) {
            m_logger->Debug("Tilemap texture generated successfully");
        }
    }

} // namespace Engine
