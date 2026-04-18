#pragma once

#include "../Core/Types.h"
#include <vector>
#include <unordered_set>
#include <cstdint>
#include <cmath>

namespace Engine {

    class Entity;

    /// Fixed-cell spatial grid for fast proximity queries.
    /// Entities register/unregister when they move between cells.
    /// Range queries only check nearby cells → O(k) instead of O(n).
    class SpatialGrid {
    public:
        SpatialGrid(uint16_t width, uint16_t height, uint16_t cellSize);
        ~SpatialGrid() = default;

        /// Insert an entity into the grid at its current position.
        void Insert(Entity* entity);

        /// Remove an entity from the grid.
        void Remove(Entity* entity);

        /// Update an entity's cell (call after position changes).
        void Update(Entity* entity, const Point& oldPos);

        /// Clear all entities from the grid.
        void Clear();

        /// Get all entities in the cell containing (x, y).
        const std::vector<Entity*>& GetEntitiesAt(int x, int y) const;

        /// Get all entities within radius of center.
        std::vector<Entity*> QueryRadius(const Point& center, float radius) const;

        /// Get all entities within a rectangle.
        std::vector<Entity*> QueryRect(const Rect& rect) const;

        uint16_t GetCellSize() const { return m_cellSize; }
        uint16_t GetGridWidth() const { return m_gridWidth; }
        uint16_t GetGridHeight() const { return m_gridHeight; }

    private:
        uint16_t PositionToCell(int coord) const;
        size_t CellIndex(uint16_t cx, uint16_t cy) const;

        uint16_t m_cellSize;
        uint16_t m_gridWidth;   // number of cells horizontally
        uint16_t m_gridHeight;  // number of cells vertically

        // Flat 2D grid of entity lists
        std::vector<std::vector<Entity*>> m_cells;

        // Empty vector returned for out-of-bounds queries
        static const std::vector<Entity*> s_empty;
    };

} // namespace Engine
