#include "SpatialGrid.h"
#include "../Entity/Entity.h"
#include <algorithm>
#include <cmath>

namespace Engine {

    const std::vector<Entity*> SpatialGrid::s_empty;

    SpatialGrid::SpatialGrid(uint16_t width, uint16_t height, uint16_t cellSize)
        : m_cellSize(cellSize > 0 ? cellSize : 1)
        , m_gridWidth((width + cellSize - 1) / cellSize)
        , m_gridHeight((height + cellSize - 1) / cellSize)
        , m_cells(static_cast<size_t>(m_gridWidth) * m_gridHeight) {
    }

    void SpatialGrid::Insert(Entity* entity) {
        if (!entity) return;
        const Point& pos = entity->GetTransform().position;
        uint16_t cx = PositionToCell(pos.x);
        uint16_t cy = PositionToCell(pos.y);
        size_t idx = CellIndex(cx, cy);
        if (idx < m_cells.size()) {
            m_cells[idx].push_back(entity);
        }
    }

    void SpatialGrid::Remove(Entity* entity) {
        if (!entity) return;
        const Point& pos = entity->GetTransform().position;
        uint16_t cx = PositionToCell(pos.x);
        uint16_t cy = PositionToCell(pos.y);
        size_t idx = CellIndex(cx, cy);
        if (idx < m_cells.size()) {
            auto& cell = m_cells[idx];
            cell.erase(std::remove(cell.begin(), cell.end(), entity), cell.end());
        }
    }

    void SpatialGrid::Update(Entity* entity, const Point& oldPos) {
        if (!entity) return;
        uint16_t oldCx = PositionToCell(oldPos.x);
        uint16_t oldCy = PositionToCell(oldPos.y);

        const Point& newPos = entity->GetTransform().position;
        uint16_t newCx = PositionToCell(newPos.x);
        uint16_t newCy = PositionToCell(newPos.y);

        if (oldCx == newCx && oldCy == newCy) return; // same cell

        // Remove from old cell
        size_t oldIdx = CellIndex(oldCx, oldCy);
        if (oldIdx < m_cells.size()) {
            auto& cell = m_cells[oldIdx];
            cell.erase(std::remove(cell.begin(), cell.end(), entity), cell.end());
        }

        // Insert into new cell
        size_t newIdx = CellIndex(newCx, newCy);
        if (newIdx < m_cells.size()) {
            m_cells[newIdx].push_back(entity);
        }
    }

    void SpatialGrid::Clear() {
        for (auto& cell : m_cells) {
            cell.clear();
        }
    }

    const std::vector<Entity*>& SpatialGrid::GetEntitiesAt(int x, int y) const {
        uint16_t cx = PositionToCell(x);
        uint16_t cy = PositionToCell(y);
        size_t idx = CellIndex(cx, cy);
        if (idx < m_cells.size()) {
            return m_cells[idx];
        }
        return s_empty;
    }

    std::vector<Entity*> SpatialGrid::QueryRadius(const Point& center, float radius) const {
        std::vector<Entity*> result;
        float radiusSq = radius * radius;

        // Determine cell range to check
        int minCx = static_cast<int>(PositionToCell(center.x - static_cast<int>(radius)));
        int minCy = static_cast<int>(PositionToCell(center.y - static_cast<int>(radius)));
        int maxCx = static_cast<int>(PositionToCell(center.x + static_cast<int>(radius)));
        int maxCy = static_cast<int>(PositionToCell(center.y + static_cast<int>(radius)));

        minCx = std::max(minCx, 0);
        minCy = std::max(minCy, 0);
        maxCx = std::min(maxCx, static_cast<int>(m_gridWidth) - 1);
        maxCy = std::min(maxCy, static_cast<int>(m_gridHeight) - 1);

        for (int cy = minCy; cy <= maxCy; ++cy) {
            for (int cx = minCx; cx <= maxCx; ++cx) {
                size_t idx = CellIndex(static_cast<uint16_t>(cx), static_cast<uint16_t>(cy));
                for (Entity* e : m_cells[idx]) {
                    const Point& pos = e->GetTransform().position;
                    float dx = static_cast<float>(pos.x - center.x);
                    float dy = static_cast<float>(pos.y - center.y);
                    if (dx * dx + dy * dy <= radiusSq) {
                        result.push_back(e);
                    }
                }
            }
        }
        return result;
    }

    std::vector<Entity*> SpatialGrid::QueryRect(const Rect& rect) const {
        std::vector<Entity*> result;

        int minCx = static_cast<int>(PositionToCell(rect.x));
        int minCy = static_cast<int>(PositionToCell(rect.y));
        int maxCx = static_cast<int>(PositionToCell(rect.x + rect.w));
        int maxCy = static_cast<int>(PositionToCell(rect.y + rect.h));

        minCx = std::max(minCx, 0);
        minCy = std::max(minCy, 0);
        maxCx = std::min(maxCx, static_cast<int>(m_gridWidth) - 1);
        maxCy = std::min(maxCy, static_cast<int>(m_gridHeight) - 1);

        for (int cy = minCy; cy <= maxCy; ++cy) {
            for (int cx = minCx; cx <= maxCx; ++cx) {
                size_t idx = CellIndex(static_cast<uint16_t>(cx), static_cast<uint16_t>(cy));
                for (Entity* e : m_cells[idx]) {
                    const Point& pos = e->GetTransform().position;
                    if (pos.x >= rect.x && pos.x < rect.x + rect.w &&
                        pos.y >= rect.y && pos.y < rect.y + rect.h) {
                        result.push_back(e);
                    }
                }
            }
        }
        return result;
    }

    uint16_t SpatialGrid::PositionToCell(int coord) const {
        if (coord < 0) return 0;
        uint16_t cell = static_cast<uint16_t>(coord / m_cellSize);
        if (cell >= (coord < m_cellSize * m_gridWidth ? m_gridWidth : m_gridWidth)) {
            return m_gridWidth > 0 ? m_gridWidth - 1 : 0;
        }
        return cell;
    }

    size_t SpatialGrid::CellIndex(uint16_t cx, uint16_t cy) const {
        return static_cast<size_t>(cy) * m_gridWidth + cx;
    }

} // namespace Engine
