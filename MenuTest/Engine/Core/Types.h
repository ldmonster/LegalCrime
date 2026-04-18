#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <cmath>
#include <cstddef>

// Common type aliases for the engine
namespace Engine {
    
    // Smart pointer aliases
    template<typename T>
    using UniquePtr = std::unique_ptr<T>;
    
    template<typename T>
    using SharedPtr = std::shared_ptr<T>;
    
    template<typename T>
    using WeakPtr = std::weak_ptr<T>;
    
    // Integer types
    using int8 = int8_t;
    using int16 = int16_t;
    using int32 = int32_t;
    using int64 = int64_t;
    
    using uint8 = uint8_t;
    using uint16 = uint16_t;
    using uint32 = uint32_t;
    using uint64 = uint64_t;
    
    // Common structures
    struct Resolution {
        uint32 width;
        uint32 height;
        
        Resolution(uint32 w = 800, uint32 h = 600) : width(w), height(h) {}
    };

    /// Display scale factor for High-DPI support.
    /// Logical coordinates (game units) are multiplied by scale to get physical pixels.
    struct DisplayScale {
        float horizontal;
        float vertical;

        DisplayScale(float h = 1.0f, float v = 1.0f) : horizontal(h), vertical(v) {}
        bool IsHighDPI() const { return horizontal > 1.0f || vertical > 1.0f; }
    };
    
    struct Color {
        uint8 r, g, b, a;

        Color(uint8 red = 255, uint8 green = 255, uint8 blue = 255, uint8 alpha = 255)
            : r(red), g(green), b(blue), a(alpha) {}
    };

    struct Point {
        int x, y;

        Point(int xPos = 0, int yPos = 0) : x(xPos), y(yPos) {}
    };

    struct Rect {
        int x, y;
        int w, h;

        Rect() : x(0), y(0), w(0), h(0) {}
        Rect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
    };

    /// Value object representing a position in the tile grid.
    struct TilePosition {
        uint16_t row;
        uint16_t col;

        TilePosition() : row(0), col(0) {}
        TilePosition(uint16_t r, uint16_t c) : row(r), col(c) {}

        bool operator==(const TilePosition& o) const { return row == o.row && col == o.col; }
        bool operator!=(const TilePosition& o) const { return !(*this == o); }

        /// Manhattan distance to another tile.
        int DistanceTo(const TilePosition& o) const {
            return std::abs(static_cast<int>(row) - o.row)
                 + std::abs(static_cast<int>(col) - o.col);
        }

        /// True if the other tile is directly adjacent (4-directional).
        bool IsAdjacent(const TilePosition& o) const { return DistanceTo(o) == 1; }

        /// Hash for use in unordered containers.
        struct Hash {
            std::size_t operator()(const TilePosition& p) const {
                return std::hash<uint32_t>()(
                    (static_cast<uint32_t>(p.row) << 16) | p.col);
            }
        };
    };

    /// Cardinal direction value object with behavior.
    enum class Direction : uint8_t {
        Down  = 0,
        Right = 1,
        Up    = 2,
        Left  = 3
    };

    namespace DirectionUtil {
        inline const char* ToString(Direction dir) {
            constexpr const char* names[] = { "down", "right", "up", "left" };
            auto idx = static_cast<uint8_t>(dir);
            return idx < 4 ? names[idx] : "down";
        }

        inline std::string ToAnimationName(Direction dir, const std::string& prefix = "walk") {
            return prefix + "_" + ToString(dir);
        }

        inline Direction Opposite(Direction dir) {
            constexpr Direction opposites[] = {
                Direction::Up, Direction::Left, Direction::Down, Direction::Right
            };
            auto idx = static_cast<uint8_t>(dir);
            return idx < 4 ? opposites[idx] : Direction::Down;
        }

        /// Determine direction from a tile delta (row/col difference).
        inline Direction FromDelta(int deltaRow, int deltaCol) {
            if (deltaRow < 0) return Direction::Up;
            if (deltaRow > 0) return Direction::Down;
            if (deltaCol < 0) return Direction::Left;
            return Direction::Right;
        }

        /// Determine direction from one TilePosition to another.
        inline Direction FromPositions(const TilePosition& from, const TilePosition& to) {
            return FromDelta(
                static_cast<int>(to.row) - static_cast<int>(from.row),
                static_cast<int>(to.col) - static_cast<int>(from.col)
            );
        }
    }

    // Result type for operations that can fail
    template<typename T>
    struct Result {
        bool success;
        T value;
        std::string error;
        
        Result() : success(false), value{}, error("") {}
        Result(T val) : success(true), value(std::move(val)), error("") {}
        Result(const std::string& err) : success(false), value{}, error(err) {}
        
        operator bool() const { return success; }

        static Result Success(T val) { return Result(std::move(val)); }
        static Result Failure(const std::string& err) { return Result(err); }
    };
    
    // Specialization for void operations
    template<>
    struct Result<void> {
        bool success;
        std::string error;
        
        Result() : success(true), error("") {}
        Result(bool s, const std::string& err = "") : success(s), error(err) {}
        
        operator bool() const { return success; }
        
        static Result Success() { return Result(true); }
        static Result Failure(const std::string& err) { return Result(false, err); }
    };
}
