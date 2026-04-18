#include "VisionSystem.h"
#include "../../../Engine/Core/Logger/ILogger.h"
#include <cmath>
#include <algorithm>

namespace LegalCrime {
namespace World {

    VisionSystem::VisionSystem(uint16_t mapWidth, uint16_t mapHeight, Engine::ILogger* logger)
        : m_logger(logger)
        , m_width(mapWidth)
        , m_height(mapHeight)
        , m_grid(static_cast<size_t>(mapWidth) * mapHeight, TileVisibility::Unexplored) {
    }

    VisionSystem::~VisionSystem() = default;

    void VisionSystem::BeginFrame() {
        for (auto& v : m_grid) {
            if (v == TileVisibility::Visible) {
                v = TileVisibility::Explored;
            }
        }
    }

    void VisionSystem::RevealAround(const Engine::TilePosition& center, uint16_t visionRadius) {
        int r = static_cast<int>(visionRadius);
        int cr = static_cast<int>(center.row);
        int cc = static_cast<int>(center.col);

        int minRow = std::max(0, cr - r);
        int maxRow = std::min(static_cast<int>(m_height) - 1, cr + r);
        int minCol = std::max(0, cc - r);
        int maxCol = std::min(static_cast<int>(m_width) - 1, cc + r);

        int r2 = r * r;

        for (int row = minRow; row <= maxRow; ++row) {
            for (int col = minCol; col <= maxCol; ++col) {
                int dr = row - cr;
                int dc = col - cc;
                if (dr * dr + dc * dc <= r2) {
                    m_grid[static_cast<size_t>(row) * m_width + col] = TileVisibility::Visible;
                }
            }
        }
    }

    TileVisibility VisionSystem::GetVisibility(uint16_t row, uint16_t col) const {
        if (row >= m_height || col >= m_width) return TileVisibility::Unexplored;
        return m_grid[static_cast<size_t>(row) * m_width + col];
    }

    TileVisibility VisionSystem::GetVisibility(const Engine::TilePosition& pos) const {
        return GetVisibility(pos.row, pos.col);
    }

    bool VisionSystem::IsVisible(uint16_t row, uint16_t col) const {
        return GetVisibility(row, col) == TileVisibility::Visible;
    }

    bool VisionSystem::IsExplored(uint16_t row, uint16_t col) const {
        auto v = GetVisibility(row, col);
        return v == TileVisibility::Explored || v == TileVisibility::Visible;
    }

    void VisionSystem::RevealAll() {
        std::fill(m_grid.begin(), m_grid.end(), TileVisibility::Visible);
    }

    void VisionSystem::Reset() {
        std::fill(m_grid.begin(), m_grid.end(), TileVisibility::Unexplored);
    }

} // namespace World
} // namespace LegalCrime
