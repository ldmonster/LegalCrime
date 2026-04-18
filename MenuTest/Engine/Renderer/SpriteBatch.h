#pragma once

#include "../Core/Types.h"
#include <SDL3/SDL.h>
#include <vector>
#include <algorithm>

namespace Engine {

    class IRenderer;

    /// Batches same-texture sprite draw calls into groups and flushes them together.
    /// Reduces GPU state changes by sorting sprites by texture before rendering.
    /// Usage:
    ///   batch.Begin();
    ///   batch.Draw(texture1, src, dst);
    ///   batch.Draw(texture2, src, dst);
    ///   batch.Draw(texture1, src, dst);  // grouped with first call
    ///   batch.End(renderer);             // flushes all batched draws
    class SpriteBatch {
    public:
        SpriteBatch();
        ~SpriteBatch() = default;

        /// Begin a new batch. Clears previous entries.
        void Begin();

        /// Queue a sprite for batched rendering.
        void Draw(SDL_Texture* texture, const Rect& srcRect, const Rect& dstRect,
                  float rotation = 0.0, SDL_FlipMode flip = SDL_FLIP_NONE);

        /// Sort by texture and flush all queued draws to the renderer.
        void End(IRenderer* renderer);

        /// Number of draw calls that were issued in the last End() call.
        int GetLastDrawCallCount() const { return m_lastDrawCallCount; }

        /// Number of sprites batched in the last End() call.
        int GetLastSpriteCount() const { return m_lastSpriteCount; }

    private:
        struct SpriteEntry {
            SDL_Texture* texture;
            SDL_FRect srcRect;
            SDL_FRect dstRect;
            float rotation;
            SDL_FlipMode flip;
        };

        std::vector<SpriteEntry> m_entries;
        int m_lastDrawCallCount;
        int m_lastSpriteCount;
    };

} // namespace Engine
