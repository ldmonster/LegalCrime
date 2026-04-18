#pragma once

#include "../../../Engine/Core/Types.h"
#include <vector>
#include <cstdint>

namespace Engine {
    class ILogger;
}

namespace LegalCrime {
namespace World {

    /// Per-tile visibility state for fog of war.
    enum class TileVisibility : uint8_t {
        Unexplored = 0,  // Never seen
        Explored   = 1,  // Previously visible, now fogged
        Visible    = 2   // Currently in view
    };

    /// VisionSystem manages per-tile visibility for fog of war.
    /// Each frame, all tiles are demoted from Visible → Explored,
    /// then units reveal tiles within their vision radius.
    class VisionSystem {
    public:
        VisionSystem(uint16_t mapWidth, uint16_t mapHeight, Engine::ILogger* logger = nullptr);
        ~VisionSystem();

        /// Reset visibility: demote all Visible tiles to Explored.
        void BeginFrame();

        /// Reveal tiles around a position with the given vision radius.
        void RevealAround(const Engine::TilePosition& center, uint16_t visionRadius);

        /// Get the visibility state of a tile.
        TileVisibility GetVisibility(uint16_t row, uint16_t col) const;
        TileVisibility GetVisibility(const Engine::TilePosition& pos) const;

        /// Check convenience methods.
        bool IsVisible(uint16_t row, uint16_t col) const;
        bool IsExplored(uint16_t row, uint16_t col) const;

        /// Make every tile visible (debug / reveal-all cheat).
        void RevealAll();

        /// Reset everything to Unexplored.
        void Reset();

        uint16_t GetWidth() const { return m_width; }
        uint16_t GetHeight() const { return m_height; }

    private:
        Engine::ILogger* m_logger;
        uint16_t m_width;
        uint16_t m_height;
        std::vector<TileVisibility> m_grid; // row-major: m_grid[row * width + col]
    };

} // namespace World
} // namespace LegalCrime
