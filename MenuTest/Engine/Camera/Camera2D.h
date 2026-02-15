#pragma once

#include "../Core/Types.h"
#include "../Core/Logger/ILogger.h"

namespace Engine {

    class Camera2D {
    public:
        Camera2D(int viewportWidth, int viewportHeight, ILogger* logger = nullptr);
        ~Camera2D();

        void Update(float deltaTime);

        // Position
        void SetPosition(int x, int y);
        void SetPosition(const Point& pos);
        Point GetPosition() const { return m_position; }
        void Translate(int dx, int dy);

        // Viewport
        void SetViewport(int width, int height);
        int GetViewportWidth() const { return m_viewportWidth; }
        int GetViewportHeight() const { return m_viewportHeight; }

        // Zoom
        void SetZoom(float zoom);
        float GetZoom() const { return m_zoom; }
        void ZoomIn(float amount = 0.1f);
        void ZoomOut(float amount = 0.1f);

        // Bounds
        void SetBounds(const Rect& bounds);
        void ClearBounds();
        bool HasBounds() const { return m_hasBounds; }

        // Following
        void Follow(const Point& target, float smoothness = 0.1f);
        void StopFollowing();

        // Panning (for user-controlled camera drag)
        void StartPan(int mouseX, int mouseY);
        void UpdatePan(int mouseX, int mouseY);
        void EndPan();
        bool IsPanning() const { return m_isPanning; }

        // Shake
        void Shake(float duration, float intensity);
        void StopShake();

        // Coordinate conversion
        Point WorldToScreen(const Point& worldPos) const;
        Point ScreenToWorld(const Point& screenPos) const;

    private:
        void ClampToBounds();
        Point CalculateShakeOffset() const;

        ILogger* m_logger;
        Point m_position;
        int m_viewportWidth;
        int m_viewportHeight;
        float m_zoom;
        float m_minZoom;
        float m_maxZoom;
        bool m_hasBounds;
        Rect m_bounds;
        bool m_isFollowing;
        Point m_followTarget;
        float m_followSmoothness;
        bool m_isPanning;
        Point m_panStartMousePos;
        Point m_panStartCameraPos;
        float m_shakeTimeRemaining;
        float m_shakeDuration;
        float m_shakeIntensity;
        uint32_t m_shakeSeed;
    };

} // namespace Engine
