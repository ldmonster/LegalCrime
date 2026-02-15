#pragma once

#include "../Core/Types.h"
#include <vector>
#include <functional>
#include <cstdint>

namespace Engine {

    // Represents a position in the tile grid
    struct TilePosition {
        uint16_t row;
        uint16_t col;

        TilePosition() : row(0), col(0) {}
        TilePosition(uint16_t r, uint16_t c) : row(r), col(c) {}

        bool operator==(const TilePosition& other) const {
            return row == other.row && col == other.col;
        }

        bool operator!=(const TilePosition& other) const {
            return !(*this == other);
        }
    };

    // Path result - list of tile positions from start to goal
    using Path = std::vector<TilePosition>;

    // Callback function to check if a tile is walkable
    // Parameters: row, col
    // Returns: true if the tile can be walked on, false otherwise
    using IsWalkableFunc = std::function<bool(uint16_t row, uint16_t col)>;

    /// <summary>
    /// A* pathfinding implementation for grid-based maps.
    /// Supports obstacles and diagonal movement.
    /// </summary>
    class Pathfinding {
    public:
        // Pathfinding options
        struct Options {
            bool allowDiagonal;      // Allow diagonal movement
            bool cutCorners;         // Allow cutting corners when moving diagonally
            float diagonalCost;      // Cost multiplier for diagonal moves (usually sqrt(2) ≈ 1.414)

            Options()
                : allowDiagonal(true)
                , cutCorners(false)
                , diagonalCost(1.414f) {}
        };

        Pathfinding();
        ~Pathfinding();

        /// <summary>
        /// Find a path from start to goal using A* algorithm.
        /// </summary>
        /// <param name="start">Starting tile position</param>
        /// <param name="goal">Goal tile position</param>
        /// <param name="mapWidth">Width of the map in tiles</param>
        /// <param name="mapHeight">Height of the map in tiles</param>
        /// <param name="isWalkable">Callback to check if a tile is walkable</param>
        /// <param name="options">Pathfinding options (diagonal movement, etc.)</param>
        /// <returns>Path from start to goal, or empty if no path found</returns>
        Path FindPath(
            const TilePosition& start,
            const TilePosition& goal,
            uint16_t mapWidth,
            uint16_t mapHeight,
            const IsWalkableFunc& isWalkable,
            const Options& options = Options()
        );

        /// <summary>
        /// Check if a path exists between start and goal.
        /// </summary>
        bool HasPath(
            const TilePosition& start,
            const TilePosition& goal,
            uint16_t mapWidth,
            uint16_t mapHeight,
            const IsWalkableFunc& isWalkable,
            const Options& options = Options()
        );

        /// <summary>
        /// Get the last path found (useful for debugging).
        /// </summary>
        const Path& GetLastPath() const { return m_lastPath; }

        /// <summary>
        /// Get statistics from the last pathfinding operation.
        /// </summary>
        struct Stats {
            int nodesExplored;
            int pathLength;
            float searchTime; // in milliseconds

            Stats() : nodesExplored(0), pathLength(0), searchTime(0.0f) {}
        };

        const Stats& GetLastStats() const { return m_lastStats; }

    private:
        // Internal node structure for A*
        struct Node {
            TilePosition position;
            float gCost;        // Distance from start
            float hCost;        // Heuristic distance to goal
            float fCost;        // gCost + hCost
            Node* parent;

            Node() : position(0, 0), gCost(0), hCost(0), fCost(0), parent(nullptr) {}
            Node(const TilePosition& pos) : position(pos), gCost(0), hCost(0), fCost(0), parent(nullptr) {}
        };

        // Calculate heuristic (Manhattan or Euclidean distance)
        float CalculateHeuristic(const TilePosition& a, const TilePosition& b, bool allowDiagonal) const;

        // Get neighbors of a tile
        std::vector<TilePosition> GetNeighbors(
            const TilePosition& pos,
            uint16_t mapWidth,
            uint16_t mapHeight,
            const IsWalkableFunc& isWalkable,
            const Options& options
        ) const;

        // Reconstruct path from goal to start using parent pointers
        Path ReconstructPath(Node* goalNode) const;

        // Convert 2D position to 1D index
        int PositionToIndex(const TilePosition& pos, uint16_t mapWidth) const;

        Path m_lastPath;
        Stats m_lastStats;
    };

} // namespace Engine
