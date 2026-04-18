#include "Minimap.h"
#include "../Renderer/IRenderer.h"
#include "../Camera/Camera2D.h"
#include "../World/TileMap.h"
#include "../Entity/Entity.h"
#include "../Core/Logger/ILogger.h"
#include <SDL3/SDL.h>

namespace Engine {

    Minimap::Minimap(int width, int height, ILogger* logger)
        : m_logger(logger)
        , m_width(width)
        , m_height(height)
        , m_screenX(0)
        , m_screenY(0)
        , m_dirty(true)
        , m_unitDotColor(0, 255, 0, 255) {
    }

    Minimap::~Minimap() = default;

    void Minimap::UpdateFromTileMap(IRenderer* renderer, const TileMap& map) {
        if (!renderer) return;

        SDL_Renderer* sdl = renderer->GetNativeRenderer();
        if (!sdl) return;

        // Create a target texture at minimap resolution
        SDL_Texture* tex = SDL_CreateTexture(sdl, SDL_PIXELFORMAT_RGBA8888,
                                              SDL_TEXTUREACCESS_TARGET, m_width, m_height);
        if (!tex) return;

        SDL_SetRenderTarget(sdl, tex);
        SDL_SetRenderDrawColor(sdl, 20, 20, 20, 255);
        SDL_RenderClear(sdl);

        // Draw each tile as a colored pixel block
        uint16_t mapW = map.GetWidth();
        uint16_t mapH = map.GetHeight();
        float scaleX = static_cast<float>(m_width) / mapW;
        float scaleY = static_cast<float>(m_height) / mapH;

        for (uint16_t row = 0; row < mapH; ++row) {
            for (uint16_t col = 0; col < mapW; ++col) {
                const Tile* tile = map.GetTile(row, col);
                if (!tile) continue;

                if (tile->IsWalkable()) {
                    SDL_SetRenderDrawColor(sdl, 60, 80, 60, 255); // Ground
                } else {
                    SDL_SetRenderDrawColor(sdl, 120, 80, 40, 255); // Wall
                }

                SDL_FRect r;
                r.x = col * scaleX;
                r.y = row * scaleY;
                r.w = scaleX + 1;
                r.h = scaleY + 1;
                SDL_RenderFillRect(sdl, &r);
            }
        }

        SDL_SetRenderTarget(sdl, nullptr);
        m_mapTexture = Texture::CreateFromSDL(tex, m_width, m_height);
        m_dirty = false;
    }

    void Minimap::Render(IRenderer* renderer, const TileMap& map,
                          const std::vector<Entity*>& entities,
                          Camera2D* camera) {
        if (!renderer) return;

        SDL_Renderer* sdl = renderer->GetNativeRenderer();
        if (!sdl) return;

        // Re-generate minimap texture if needed
        if (m_dirty || !m_mapTexture) {
            UpdateFromTileMap(renderer, map);
        }

        // Draw minimap background
        if (m_mapTexture) {
            Rect dest(m_screenX, m_screenY, m_width, m_height);
            Rect src(0, 0, m_width, m_height);
            m_mapTexture->Render(renderer, dest, &src);
        }

        // Draw border
        renderer->SetDrawColor(200, 200, 200, 255);
        Point border[5] = {
            {m_screenX, m_screenY},
            {m_screenX + m_width, m_screenY},
            {m_screenX + m_width, m_screenY + m_height},
            {m_screenX, m_screenY + m_height},
            {m_screenX, m_screenY}
        };
        renderer->DrawLines(border, 5);

        // Draw unit dots
        uint16_t mapW = map.GetWidth();
        uint16_t mapH = map.GetHeight();
        float scaleX = static_cast<float>(m_width) / mapW;
        float scaleY = static_cast<float>(m_height) / mapH;

        SDL_SetRenderDrawColor(sdl, m_unitDotColor.r, m_unitDotColor.g,
                                m_unitDotColor.b, m_unitDotColor.a);
        for (auto* entity : entities) {
            if (!entity || !entity->IsActive()) continue;
            Point pos = entity->GetPosition();

            // Approximate tile position from screen position — use the map's center offset
            // For simplicity, map pixel position proportionally to minimap
            float mapPixelW = static_cast<float>(map.GetMapSizeWidth());
            float mapPixelH = static_cast<float>(map.GetMapSizeHeight());
            if (mapPixelW <= 0 || mapPixelH <= 0) continue;

            // Very rough mapping: entity screen pos → minimap pos
            float relX = static_cast<float>(pos.x) / mapPixelW;
            float relY = static_cast<float>(pos.y) / mapPixelH;
            float dotX = m_screenX + relX * m_width;
            float dotY = m_screenY + relY * m_height;

            SDL_FRect dotRect = {dotX - 1, dotY - 1, 3, 3};
            SDL_RenderFillRect(sdl, &dotRect);
        }

        // Draw camera viewport rectangle
        if (camera) {
            float mapPixelW = static_cast<float>(map.GetMapSizeWidth());
            float mapPixelH = static_cast<float>(map.GetMapSizeHeight());
            if (mapPixelW > 0 && mapPixelH > 0) {
                Point camPos = camera->GetPosition();
                int vpW = camera->GetViewportWidth();
                int vpH = camera->GetViewportHeight();

                float camLeft = static_cast<float>(camPos.x) / mapPixelW;
                float camTop = static_cast<float>(camPos.y) / mapPixelH;
                float camW = static_cast<float>(vpW) / mapPixelW;
                float camH = static_cast<float>(vpH) / mapPixelH;

                renderer->SetDrawColor(255, 255, 255, 200);
                Point vpRect[5] = {
                    {m_screenX + static_cast<int>(camLeft * m_width), m_screenY + static_cast<int>(camTop * m_height)},
                    {m_screenX + static_cast<int>((camLeft + camW) * m_width), m_screenY + static_cast<int>(camTop * m_height)},
                    {m_screenX + static_cast<int>((camLeft + camW) * m_width), m_screenY + static_cast<int>((camTop + camH) * m_height)},
                    {m_screenX + static_cast<int>(camLeft * m_width), m_screenY + static_cast<int>((camTop + camH) * m_height)},
                    {m_screenX + static_cast<int>(camLeft * m_width), m_screenY + static_cast<int>(camTop * m_height)}
                };
                renderer->DrawLines(vpRect, 5);
            }
        }
    }

    bool Minimap::ContainsPoint(int screenX, int screenY) const {
        return screenX >= m_screenX && screenX < m_screenX + m_width &&
               screenY >= m_screenY && screenY < m_screenY + m_height;
    }

    Point Minimap::ScreenToWorld(int screenX, int screenY, const TileMap& map) const {
        float relX = static_cast<float>(screenX - m_screenX) / m_width;
        float relY = static_cast<float>(screenY - m_screenY) / m_height;
        int worldX = static_cast<int>(relX * map.GetMapSizeWidth());
        int worldY = static_cast<int>(relY * map.GetMapSizeHeight());
        return Point(worldX, worldY);
    }

} // namespace Engine
