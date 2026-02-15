#pragma once

#include "../Core/Types.h"

namespace Engine {

    /// <summary>
    /// Represents position, rotation, and scale in 2D space.
    /// Plain-Old-Data (POD) struct for entity transformations.
    /// </summary>
    struct Transform {
        Point position;     // World position (x, y)
        float rotation;     // Rotation in degrees (0-360)
        float scale;        // Uniform scale (1.0 = normal size)

        Transform()
            : position{0, 0}
            , rotation(0.0f)
            , scale(1.0f) {
        }

        Transform(int x, int y, float rot = 0.0f, float scl = 1.0f)
            : position{x, y}
            , rotation(rot)
            , scale(scl) {
        }

        Transform(const Point& pos, float rot = 0.0f, float scl = 1.0f)
            : position(pos)
            , rotation(rot)
            , scale(scl) {
        }

        // Helper methods
        void SetPosition(int x, int y) {
            position.x = x;
            position.y = y;
        }

        void SetPosition(const Point& pos) {
            position = pos;
        }

        void Translate(int dx, int dy) {
            position.x += dx;
            position.y += dy;
        }

        void Rotate(float degrees) {
            rotation += degrees;
            // Normalize to 0-360
            while (rotation >= 360.0f) rotation -= 360.0f;
            while (rotation < 0.0f) rotation += 360.0f;
        }

        void SetRotation(float degrees) {
            rotation = degrees;
            // Normalize to 0-360
            while (rotation >= 360.0f) rotation -= 360.0f;
            while (rotation < 0.0f) rotation += 360.0f;
        }

        void SetScale(float scl) {
            scale = scl;
        }

        void ScaleBy(float factor) {
            scale *= factor;
        }
    };

} // namespace Engine
