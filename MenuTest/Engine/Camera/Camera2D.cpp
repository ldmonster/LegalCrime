#include "Camera2D.h"
#include <cmath>
#include <algorithm>

namespace Engine {

    Camera2D::Camera2D(int viewportWidth, int viewportHeight, ILogger* logger)
        : m_logger(logger)
        , m_position(0, 0)
        , m_viewportWidth(viewportWidth)
        , m_viewportHeight(viewportHeight)
        , m_zoom(1.0f)
        , m_minZoom(0.1f)
        , m_maxZoom(5.0f)
        , m_hasBounds(false)
        , m_bounds(0, 0, 0, 0)
        , m_isFollowing(false)
        , m_followTarget(0, 0)
        , m_followSmoothness(0.1f)
        , m_shakeTimeRemaining(0.0f)
        , m_shakeDuration(0.0f)
        , m_shakeIntensity(0.0f)
        , m_shakeSeed(0) {
        
        if (m_logger) {
            m_logger->Debug("Camera2D created with viewport: " + 
                          std::to_string(viewportWidth) + "x" + std::to_string(viewportHeight));
        }
    }

    Camera2D::~Camera2D() {
        if (m_logger) {
            m_logger->Debug("Camera2D destroyed");
        }
    }

    void Camera2D::Update(float deltaTime) {
        // Update smooth following
        if (m_isFollowing) {
            float dx = static_cast<float>(m_followTarget.x - m_position.x) * m_followSmoothness;
            float dy = static_cast<float>(m_followTarget.y - m_position.y) * m_followSmoothness;
            
            m_position.x += static_cast<int>(dx);
            m_position.y += static_cast<int>(dy);
            
            ClampToBounds();
        }

        // Update shake
        if (m_shakeTimeRemaining > 0.0f) {
            m_shakeTimeRemaining -= deltaTime;
            if (m_shakeTimeRemaining < 0.0f) {
                m_shakeTimeRemaining = 0.0f;
            }
        }
    }

    void Camera2D::SetPosition(int x, int y) {
        m_position.x = x;
        m_position.y = y;
        ClampToBounds();
    }

    void Camera2D::SetPosition(const Point& pos) {
        m_position = pos;
        ClampToBounds();
    }

    void Camera2D::Translate(int dx, int dy) {
        m_position.x += dx;
        m_position.y += dy;
        ClampToBounds();
    }

    void Camera2D::SetViewport(int width, int height) {
        m_viewportWidth = width;
        m_viewportHeight = height;
    }

    void Camera2D::SetZoom(float zoom) {
        m_zoom = std::clamp(zoom, m_minZoom, m_maxZoom);
    }

    void Camera2D::ZoomIn(float amount) {
        SetZoom(m_zoom + amount);
    }

    void Camera2D::ZoomOut(float amount) {
        SetZoom(m_zoom - amount);
    }

    void Camera2D::SetBounds(const Rect& bounds) {
        m_bounds = bounds;
        m_hasBounds = true;
        ClampToBounds();
        
        if (m_logger) {
            m_logger->Debug("Camera2D bounds set to: (" + 
                          std::to_string(bounds.x) + ", " + std::to_string(bounds.y) + ", " +
                          std::to_string(bounds.w) + ", " + std::to_string(bounds.h) + ")");
        }
    }

    void Camera2D::ClearBounds() {
        m_hasBounds = false;
    }

    void Camera2D::Follow(const Point& target, float smoothness) {
        m_followTarget = target;
        m_followSmoothness = std::clamp(smoothness, 0.01f, 1.0f);
        m_isFollowing = true;
    }

    void Camera2D::StopFollowing() {
        m_isFollowing = false;
    }

    void Camera2D::Shake(float duration, float intensity) {
        m_shakeDuration = duration;
        m_shakeTimeRemaining = duration;
        m_shakeIntensity = intensity;
        m_shakeSeed = static_cast<uint32_t>(m_position.x + m_position.y);
    }

    void Camera2D::StopShake() {
        m_shakeTimeRemaining = 0.0f;
    }

    Point Camera2D::WorldToScreen(const Point& worldPos) const {
        Point shakeOffset = CalculateShakeOffset();
        
        int screenX = static_cast<int>((worldPos.x - m_position.x) * m_zoom) + 
                     m_viewportWidth / 2 + shakeOffset.x;
        int screenY = static_cast<int>((worldPos.y - m_position.y) * m_zoom) + 
                     m_viewportHeight / 2 + shakeOffset.y;
        
        return Point(screenX, screenY);
    }

    Point Camera2D::ScreenToWorld(const Point& screenPos) const {
        Point shakeOffset = CalculateShakeOffset();
        
        int worldX = static_cast<int>((screenPos.x - m_viewportWidth / 2 - shakeOffset.x) / m_zoom) + 
                    m_position.x;
        int worldY = static_cast<int>((screenPos.y - m_viewportHeight / 2 - shakeOffset.y) / m_zoom) + 
                    m_position.y;
        
        return Point(worldX, worldY);
    }

    void Camera2D::ClampToBounds() {
        if (!m_hasBounds) {
            return;
        }

        // Calculate half viewport size in world space
        int halfWidth = static_cast<int>(m_viewportWidth / (2.0f * m_zoom));
        int halfHeight = static_cast<int>(m_viewportHeight / (2.0f * m_zoom));

        // Calculate bounds accounting for viewport
        int minX = m_bounds.x + halfWidth;
        int maxX = m_bounds.x + m_bounds.w - halfWidth;
        int minY = m_bounds.y + halfHeight;
        int maxY = m_bounds.y + m_bounds.h - halfHeight;

        // Center camera if viewport is larger than bounds
        if (m_viewportWidth / m_zoom >= m_bounds.w) {
            m_position.x = m_bounds.x + m_bounds.w / 2;
        } else {
            m_position.x = std::clamp(m_position.x, minX, maxX);
        }

        if (m_viewportHeight / m_zoom >= m_bounds.h) {
            m_position.y = m_bounds.y + m_bounds.h / 2;
        } else {
            m_position.y = std::clamp(m_position.y, minY, maxY);
        }
    }

    Point Camera2D::CalculateShakeOffset() const {
        if (m_shakeTimeRemaining <= 0.0f) {
            return Point(0, 0);
        }

        // Simple pseudo-random shake using seed
        float t = m_shakeTimeRemaining / m_shakeDuration;
        float intensity = m_shakeIntensity * t;  // Decay over time

        // Use sine waves for smooth shake
        int offsetX = static_cast<int>(std::sin(m_shakeTimeRemaining * 50.0f + m_shakeSeed) * intensity);
        int offsetY = static_cast<int>(std::cos(m_shakeTimeRemaining * 45.0f + m_shakeSeed) * intensity);

        return Point(offsetX, offsetY);
    }

} // namespace Engine
