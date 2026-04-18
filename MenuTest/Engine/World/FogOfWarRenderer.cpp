#include "FogOfWarRenderer.h"
#include "TileMap.h"
#include "IsometricMath.h"
#include "../Renderer/IRenderer.h"
#include "../Camera/Camera2D.h"
#include "../Core/Logger/ILogger.h"
#include "../../Game/World/Systems/VisionSystem.h"
#include <SDL3/SDL.h>

namespace Engine {

    FogOfWarRenderer::FogOfWarRenderer(ILogger* logger)
        : m_logger(logger)
        , m_enabled(true) {
    }

    FogOfWarRenderer::~FogOfWarRenderer() = default;

    void FogOfWarRenderer::Render(IRenderer* renderer, const TileMap& map,
                                   const LegalCrime::World::VisionSystem& vision,
                                   Camera2D* camera) {
        if (!m_enabled || !renderer) return;

        SDL_Renderer* sdlRenderer = renderer->GetNativeRenderer();
        if (!sdlRenderer) return;

        // Enable blending for semi-transparent fog
        SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BLENDMODE_BLEND);

        uint16_t mapW = map.GetWidth();
        uint16_t mapH = map.GetHeight();

        for (uint16_t row = 0; row < mapH; ++row) {
            for (uint16_t col = 0; col < mapW; ++col) {
                auto vis = vision.GetVisibility(row, col);
                if (vis == LegalCrime::World::TileVisibility::Visible) continue;

                // Get tile screen position
                Point tileScreen = map.TileToScreen(row, col, camera);
                int tw = map.GetTileWidth();
                int th = map.GetTileHeight();

                // Draw isometric diamond as filled fog
                // Diamond points: top, right, bottom, left
                float cx = tileScreen.x + tw;
                float cy = static_cast<float>(tileScreen.y);
                SDL_FPoint diamond[4] = {
                    {cx,              cy - th * 0.5f},  // top
                    {cx + tw,         cy},               // right
                    {cx,              cy + th * 0.5f},  // bottom
                    {cx - tw,         cy}                // left
                };

                uint8_t alpha = (vis == LegalCrime::World::TileVisibility::Unexplored) ? 255 : 140;
                SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, alpha);

                // Fill with two triangles
                SDL_FPoint tri1[3] = {diamond[0], diamond[1], diamond[2]};
                SDL_FPoint tri2[3] = {diamond[0], diamond[2], diamond[3]};

                // Use SDL_RenderGeometry for filled triangles
                SDL_Vertex verts[6];
                SDL_FColor fogColor = {0.0f, 0.0f, 0.0f, alpha / 255.0f};
                for (int i = 0; i < 3; ++i) {
                    verts[i].position = tri1[i];
                    verts[i].color = fogColor;
                    verts[i].tex_coord = {0, 0};
                }
                for (int i = 0; i < 3; ++i) {
                    verts[3 + i].position = tri2[i];
                    verts[3 + i].color = fogColor;
                    verts[3 + i].tex_coord = {0, 0};
                }

                SDL_RenderGeometry(sdlRenderer, nullptr, verts, 6, nullptr, 0);
            }
        }
    }

} // namespace Engine
