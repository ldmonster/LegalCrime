#pragma once

#include "../Core/Types.h"
#include <cmath>
#include <cstdint>

namespace Engine {

    /// Shared isometric coordinate conversion utilities.
    /// Used by TileMap, TileMapRenderer, Camera2D, and game systems.
    class IsometricMath {
    public:
        /// Convert tile grid (row, col) to screen-space pixel coordinates (top-left of diamond).
        /// @param row Tile row
        /// @param col Tile column
        /// @param originX X origin in texture/map space (usually 0)
        /// @param originY Y origin in texture/map space (usually mapWidth * tileHeight)
        /// @param tileWidth Width of isometric tile in pixels
        /// @param tileHeight Height of isometric tile in pixels
        static Point TileToLocal(uint16_t row, uint16_t col,
                                 int originX, int originY,
                                 int tileWidth, int tileHeight) {
            int x = originX + (col + row) * tileWidth;
            int y = originY - (col - row) * tileHeight;
            return Point(x, y);
        }

        /// Convert screen-space pixel coordinates to tile grid (row, col).
        /// Returns false if the position is outside the map bounds.
        static bool LocalToTile(int localX, int localY,
                                int originX, int originY,
                                int tileWidth, int tileHeight,
                                uint16_t mapWidth, uint16_t mapHeight,
                                uint16_t& outRow, uint16_t& outCol) {
            // Adjust for diamond center (better click accuracy)
            int adjustedX = localX - tileWidth;
            int adjustedY = localY;

            int offsetX = adjustedX - originX;
            int offsetY = originY - adjustedY;

            float colPlusRow = static_cast<float>(offsetX) / tileWidth;
            float colMinusRow = static_cast<float>(offsetY) / tileHeight;

            float col = (colPlusRow + colMinusRow) / 2.0f;
            float row = (colPlusRow - colMinusRow) / 2.0f;

            int tileCol = static_cast<int>(std::round(col));
            int tileRow = static_cast<int>(std::round(row));

            if (tileRow < 0 || tileRow >= mapHeight || tileCol < 0 || tileCol >= mapWidth) {
                return false;
            }

            outRow = static_cast<uint16_t>(tileRow);
            outCol = static_cast<uint16_t>(tileCol);
            return true;
        }

        /// Get the center of a tile's diamond in local/screen space.
        static Point TileToLocalCenter(uint16_t row, uint16_t col,
                                       int originX, int originY,
                                       int tileWidth, int tileHeight) {
            Point topLeft = TileToLocal(row, col, originX, originY, tileWidth, tileHeight);
            topLeft.x += tileWidth;
            return topLeft;
        }

        /// Calculate the total map size in pixels for an isometric map.
        static void CalculateMapSize(uint16_t mapWidth, uint16_t mapHeight,
                                     uint16_t tileWidth, uint16_t tileHeight,
                                     uint16_t& outPixelWidth, uint16_t& outPixelHeight) {
            outPixelWidth = (mapWidth + mapHeight) * tileWidth;
            outPixelHeight = (mapWidth + mapHeight) * tileHeight;
        }

        /// Calculate diamond bounds clamping using |x|/w + |y|/h <= 1 equation.
        /// Modifies offsetX/offsetY in-place if they exceed the diamond bounds.
        static void ClampToDiamondBounds(int& offsetX, int& offsetY,
                                         uint16_t mapWidth, uint16_t mapHeight,
                                         uint16_t tileWidth, uint16_t tileHeight) {
            float diamondWidth = (mapWidth - 1 + mapHeight - 1) * tileWidth;
            float diamondHeight = (mapWidth - 1 + mapHeight - 1) * tileHeight;

            float aspectRatio = static_cast<float>(
                mapWidth > mapHeight ? mapWidth : mapHeight) /
                static_cast<float>(
                    mapWidth < mapHeight ? mapWidth : mapHeight);

            float boundMultiplier = 1.0f + (std::min(aspectRatio, 10.0f) - 1.0f) / 9.0f;
            float boundHalfWidth = (diamondWidth / 2.0f) * boundMultiplier;
            float boundHalfHeight = (diamondHeight / 2.0f) * boundMultiplier;

            float normalizedX = std::abs(static_cast<float>(offsetX)) / boundHalfWidth;
            float normalizedY = std::abs(static_cast<float>(offsetY)) / boundHalfHeight;
            float distance = normalizedX + normalizedY;

            if (distance > 1.0f) {
                float scale = 1.0f / distance;
                offsetX = static_cast<int>(offsetX * scale);
                offsetY = static_cast<int>(offsetY * scale);
            }
        }
    };

} // namespace Engine
