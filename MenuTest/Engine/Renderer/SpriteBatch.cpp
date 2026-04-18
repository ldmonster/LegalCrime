#include "SpriteBatch.h"
#include "IRenderer.h"
#include <algorithm>

namespace Engine {

    SpriteBatch::SpriteBatch()
        : m_lastDrawCallCount(0)
        , m_lastSpriteCount(0) {
    }

    void SpriteBatch::Begin() {
        m_entries.clear();
    }

    void SpriteBatch::Draw(SDL_Texture* texture, const Rect& srcRect, const Rect& dstRect,
                           float rotation, SDL_FlipMode flip) {
        if (!texture) return;

        SpriteEntry entry;
        entry.texture = texture;
        entry.srcRect = { static_cast<float>(srcRect.x), static_cast<float>(srcRect.y),
                          static_cast<float>(srcRect.w), static_cast<float>(srcRect.h) };
        entry.dstRect = { static_cast<float>(dstRect.x), static_cast<float>(dstRect.y),
                          static_cast<float>(dstRect.w), static_cast<float>(dstRect.h) };
        entry.rotation = rotation;
        entry.flip = flip;
        m_entries.push_back(entry);
    }

    void SpriteBatch::End(IRenderer* renderer) {
        if (!renderer || m_entries.empty()) {
            m_lastDrawCallCount = 0;
            m_lastSpriteCount = 0;
            return;
        }

        m_lastSpriteCount = static_cast<int>(m_entries.size());

        // Sort by texture pointer to group same-texture sprites together
        std::sort(m_entries.begin(), m_entries.end(),
            [](const SpriteEntry& a, const SpriteEntry& b) {
                return a.texture < b.texture;
            });

        SDL_Renderer* sdlRenderer = renderer->GetNativeRenderer();
        if (!sdlRenderer) return;

        int drawCalls = 0;

        for (size_t i = 0; i < m_entries.size(); ++i) {
            const SpriteEntry& e = m_entries[i];
            SDL_RenderTextureRotated(sdlRenderer, e.texture,
                                     &e.srcRect, &e.dstRect,
                                     e.rotation, nullptr, e.flip);
            drawCalls++;
        }

        m_lastDrawCallCount = drawCalls;
        m_entries.clear();
    }

} // namespace Engine
