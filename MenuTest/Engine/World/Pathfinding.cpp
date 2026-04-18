#include "Pathfinding.h"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace Engine {

    Pathfinding::Pathfinding(size_t poolCapacity)
        : m_nodePool(poolCapacity) {
    }

    Pathfinding::~Pathfinding() {
    }

    Path Pathfinding::FindPath(
        const TilePosition& start,
        const TilePosition& goal,
        uint16_t mapWidth,
        uint16_t mapHeight,
        const IsWalkableFunc& isWalkable,
        const Options& options
    ) {
        auto startTime = std::chrono::high_resolution_clock::now();

        m_lastPath.clear();
        m_lastStats = Stats();

        // Validate input
        if (!isWalkable) {
            return m_lastPath;
        }

        if (start.row >= mapHeight || start.col >= mapWidth ||
            goal.row >= mapHeight || goal.col >= mapWidth) {
            return m_lastPath;
        }

        // Check if start and goal are walkable
        if (!isWalkable(start) || !isWalkable(goal)) {
            return m_lastPath;
        }

        // If start == goal, return path with just the start
        if (start == goal) {
            m_lastPath.push_back(start);
            m_lastStats.pathLength = 1;
            return m_lastPath;
        }

        // A* algorithm
        using NodeMap = std::unordered_map<TilePosition, Node*, TilePosition::Hash>;
        using NodeSet = std::unordered_set<TilePosition, TilePosition::Hash>;

        // Comparison for priority queue (min-heap based on fCost)
        auto nodeComparator = [](const Node* a, const Node* b) {
            return a->fCost > b->fCost; // Min-heap: lower fCost has higher priority
        };

        // Ensure pool is large enough for this map and reset it
        size_t maxNodes = static_cast<size_t>(mapWidth) * mapHeight;
        m_nodePool.Reserve(maxNodes);
        m_nodePool.Reset();

        NodeMap allNodes;
        NodeSet closedSet;
        std::priority_queue<Node*, std::vector<Node*>, decltype(nodeComparator)> openSet(nodeComparator);

        // Create start node from pool
        Node* startNode = m_nodePool.Acquire();
        if (!startNode) return m_lastPath;
        startNode->position = start;
        startNode->gCost = 0;
        startNode->hCost = CalculateHeuristic(start, goal, options.allowDiagonal);
        startNode->fCost = startNode->gCost + startNode->hCost;
        startNode->parent = nullptr;

        allNodes[start] = startNode;
        openSet.push(startNode);

        Node* goalNode = nullptr;
        int nodesExplored = 0;

        while (!openSet.empty()) {
            // Get node with lowest fCost
            Node* current = openSet.top();
            openSet.pop();

            // Check if already processed (can happen with priority queue)
            if (closedSet.find(current->position) != closedSet.end()) {
                continue;
            }

            // Mark as processed
            closedSet.insert(current->position);
            nodesExplored++;

            // Check if we reached the goal
            if (current->position == goal) {
                goalNode = current;
                break;
            }

            // Get neighbors
            std::vector<TilePosition> neighbors = GetNeighbors(
                current->position,
                mapWidth,
                mapHeight,
                isWalkable,
                options
            );

            for (const TilePosition& neighborPos : neighbors) {
                // Skip if already processed
                if (closedSet.find(neighborPos) != closedSet.end()) {
                    continue;
                }

                // Calculate cost to neighbor
                float moveCost = 1.0f;

                // Check if diagonal movement
                int rowDiff = abs(static_cast<int>(neighborPos.row) - static_cast<int>(current->position.row));
                int colDiff = abs(static_cast<int>(neighborPos.col) - static_cast<int>(current->position.col));
                if (rowDiff + colDiff > 1) {
                    moveCost = options.diagonalCost;
                }

                float tentativeGCost = current->gCost + moveCost;

                // Get or create neighbor node
                Node* neighborNode = nullptr;
                auto it = allNodes.find(neighborPos);
                if (it != allNodes.end()) {
                    neighborNode = it->second;
                } else {
                    neighborNode = m_nodePool.Acquire();
                    if (!neighborNode) {
                        m_lastStats.poolExhausted = true;
                        break; // pool exhausted
                    }
                    neighborNode->position = neighborPos;
                    neighborNode->hCost = CalculateHeuristic(neighborPos, goal, options.allowDiagonal);
                    allNodes[neighborPos] = neighborNode;
                }

                // Update if we found a better path
                if (it == allNodes.end() || tentativeGCost < neighborNode->gCost) {
                    neighborNode->gCost = tentativeGCost;
                    neighborNode->fCost = neighborNode->gCost + neighborNode->hCost;
                    neighborNode->parent = current;

                    // Add to open set
                    openSet.push(neighborNode);
                }
            }
        }

        // Reconstruct path if found
        if (goalNode) {
            m_lastPath = ReconstructPath(goalNode);
            m_lastStats.pathLength = static_cast<int>(m_lastPath.size());
        }

        m_lastStats.nodesExplored = nodesExplored;

        // No cleanup needed — pool owns memory and resets on next call

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        m_lastStats.searchTime = duration.count() / 1000.0f; // Convert to milliseconds

        return m_lastPath;
    }

    bool Pathfinding::HasPath(
        const TilePosition& start,
        const TilePosition& goal,
        uint16_t mapWidth,
        uint16_t mapHeight,
        const IsWalkableFunc& isWalkable,
        const Options& options
    ) {
        Path path = FindPath(start, goal, mapWidth, mapHeight, isWalkable, options);
        return !path.empty();
    }

    float Pathfinding::CalculateHeuristic(const TilePosition& a, const TilePosition& b, bool allowDiagonal) const {
        int dx = abs(static_cast<int>(a.col) - static_cast<int>(b.col));
        int dy = abs(static_cast<int>(a.row) - static_cast<int>(b.row));

        if (allowDiagonal) {
            // Diagonal distance (Chebyshev distance with diagonal cost)
            // D * max(dx, dy) + (D2 - D) * min(dx, dy)
            // where D = 1.0 (orthogonal cost) and D2 = 1.414 (diagonal cost)
            return static_cast<float>(std::max(dx, dy)) + 0.414f * static_cast<float>(std::min(dx, dy));
        } else {
            // Manhattan distance
            return static_cast<float>(dx + dy);
        }
    }

    std::vector<TilePosition> Pathfinding::GetNeighbors(
        const TilePosition& pos,
        uint16_t mapWidth,
        uint16_t mapHeight,
        const IsWalkableFunc& isWalkable,
        const Options& options
    ) const {
        std::vector<TilePosition> neighbors;

        // 4 cardinal directions
        struct Direction {
            int rowOffset;
            int colOffset;
            bool isDiagonal;
        };

        const Direction directions[] = {
            { -1,  0, false },  // Up
            {  1,  0, false },  // Down
            {  0, -1, false },  // Left
            {  0,  1, false },  // Right
            { -1, -1, true  },  // Up-Left
            { -1,  1, true  },  // Up-Right
            {  1, -1, true  },  // Down-Left
            {  1,  1, true  }   // Down-Right
        };

        int directionCount = options.allowDiagonal ? 8 : 4;

        for (int i = 0; i < directionCount; ++i) {
            const Direction& dir = directions[i];

            int newRow = static_cast<int>(pos.row) + dir.rowOffset;
            int newCol = static_cast<int>(pos.col) + dir.colOffset;

            // Check bounds
            if (newRow < 0 || newRow >= mapHeight || newCol < 0 || newCol >= mapWidth) {
                continue;
            }

            TilePosition neighborPos(static_cast<uint16_t>(newRow), static_cast<uint16_t>(newCol));

            // Check if walkable
            if (!isWalkable(neighborPos)) {
                continue;
            }

            // For diagonal movement, optionally check if we're cutting corners
            if (dir.isDiagonal && !options.cutCorners) {
                // Check if both adjacent orthogonal tiles are walkable
                int adjacentRow1 = static_cast<int>(pos.row) + dir.rowOffset;
                int adjacentCol1 = static_cast<int>(pos.col);
                int adjacentRow2 = static_cast<int>(pos.row);
                int adjacentCol2 = static_cast<int>(pos.col) + dir.colOffset;

                bool adjacent1Walkable = isWalkable(TilePosition(static_cast<uint16_t>(adjacentRow1), static_cast<uint16_t>(adjacentCol1)));
                bool adjacent2Walkable = isWalkable(TilePosition(static_cast<uint16_t>(adjacentRow2), static_cast<uint16_t>(adjacentCol2)));

                // Only allow diagonal if at least one adjacent tile is walkable
                if (!adjacent1Walkable && !adjacent2Walkable) {
                    continue;
                }
            }

            neighbors.push_back(neighborPos);
        }

        return neighbors;
    }

    Path Pathfinding::ReconstructPath(Node* goalNode) const {
        Path path;

        Node* current = goalNode;
        while (current != nullptr) {
            path.push_back(current->position);
            current = current->parent;
        }

        // Reverse to get path from start to goal
        std::reverse(path.begin(), path.end());

        return path;
    }

    int Pathfinding::PositionToIndex(const TilePosition& pos, uint16_t mapWidth) const {
        return pos.row * mapWidth + pos.col;
    }

    Path Pathfinding::SmoothPath(
        const Path& path,
        uint16_t mapWidth,
        uint16_t mapHeight,
        const IsWalkableFunc& isWalkable) {

        if (path.size() <= 2) return path;

        Path smoothed;
        smoothed.push_back(path.front());

        size_t current = 0;
        while (current < path.size() - 1) {
            // Try to skip ahead as far as possible
            size_t farthest = current + 1;
            for (size_t candidate = path.size() - 1; candidate > current + 1; --candidate) {
                // Bresenham line-of-sight check between current and candidate
                int r0 = path[current].row, c0 = path[current].col;
                int r1 = path[candidate].row, c1 = path[candidate].col;
                int dr = std::abs(r1 - r0), dc = std::abs(c1 - c0);
                int sr = (r0 < r1) ? 1 : -1;
                int sc = (c0 < c1) ? 1 : -1;
                int err = dr - dc;
                int r = r0, c = c0;
                bool clear = true;

                while (r != r1 || c != c1) {
                    if (r < 0 || r >= mapHeight || c < 0 || c >= mapWidth ||
                        !isWalkable(TilePosition(static_cast<uint16_t>(r), static_cast<uint16_t>(c)))) {
                        clear = false;
                        break;
                    }
                    int e2 = 2 * err;
                    if (e2 > -dc) { err -= dc; r += sr; }
                    if (e2 < dr) { err += dr; c += sc; }
                }

                if (clear) {
                    farthest = candidate;
                    break;
                }
            }
            smoothed.push_back(path[farthest]);
            current = farthest;
        }

        return smoothed;
    }

} // namespace Engine
