#pragma once

#include "IRenderer.h"
#include "../Core/Types.h"
#include <vector>
#include <cmath>

namespace Engine {

    // Pre-computes unit circle mesh once at construction.
    // Call DrawCircle() to render at any center/radius without repeating sin/cos.
    class ShapeCache {
    public:
        explicit ShapeCache(int circleSegments = 32) {
            if (circleSegments < 8) {
                circleSegments = 8;
            }
            m_unitCircle.resize(static_cast<size_t>(circleSegments) + 1);
            for (int i = 0; i <= circleSegments; ++i) {
                float angle = (static_cast<float>(i) * 2.0f * 3.14159265358979f)
                              / static_cast<float>(circleSegments);
                m_unitCircle[static_cast<size_t>(i)].fx = std::cos(angle);
                m_unitCircle[static_cast<size_t>(i)].fy = std::sin(angle);
            }
        }

        void DrawCircle(IRenderer& renderer, Point center, int radius, Color color) {
            if (&renderer == nullptr || radius <= 0 || m_unitCircle.empty()) {
                return;
            }

            renderer.SetDrawColor(color.r, color.g, color.b, color.a);

            const int count = static_cast<int>(m_unitCircle.size());
            std::vector<Point> points(static_cast<size_t>(count));
            for (int i = 0; i < count; ++i) {
                points[static_cast<size_t>(i)].x = center.x + static_cast<int>(radius * m_unitCircle[static_cast<size_t>(i)].fx);
                points[static_cast<size_t>(i)].y = center.y + static_cast<int>(radius * m_unitCircle[static_cast<size_t>(i)].fy);
            }

            renderer.DrawLines(points.data(), count);
        }

        int GetSegmentCount() const {
            return m_unitCircle.empty() ? 0 : static_cast<int>(m_unitCircle.size()) - 1;
        }

    private:
        struct UnitPoint {
            float fx = 0.0f;
            float fy = 0.0f;
        };
        std::vector<UnitPoint> m_unitCircle;
    };

} // namespace Engine
