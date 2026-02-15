#pragma once

#include "../Graphics/Sprite.h"
#include "../Graphics/Texture.h"
#include <SDL3/SDL.h>
#include <functional>
#include <memory>

namespace Engine {
    
    class IRenderer;
    class IAudioEngine;
    
    namespace UI {
        
        // Button states
        enum class ButtonState {
            Normal,
            Hovered,
            Pressed,
            Disabled
        };
        
        // Modern UI Button with event callbacks
        class Button {
        public:
            Button();
            ~Button() = default;
            
            // Setup sprites for different states
            void SetNormalSprite(std::shared_ptr<Sprite> sprite) { m_normalSprite = sprite; }
            void SetHoveredSprite(std::shared_ptr<Sprite> sprite) { m_hoveredSprite = sprite; }
            void SetPressedSprite(std::shared_ptr<Sprite> sprite) { m_pressedSprite = sprite; }
            void SetDisabledSprite(std::shared_ptr<Sprite> sprite) { m_disabledSprite = sprite; }
            
            // Position and size
            void SetBounds(const Rect& bounds) { m_bounds = bounds; }
            const Rect& GetBounds() const { return m_bounds; }
            
            // State management
            void SetEnabled(bool enabled) { m_enabled = enabled; }
            bool IsEnabled() const { return m_enabled; }
            ButtonState GetState() const { return m_state; }
            
            // Event callbacks
            void SetOnClick(std::function<void()> callback) { m_onClick = callback; }
            void SetOnHover(std::function<void()> callback) { m_onHover = callback; }
            void SetOnPress(std::function<void()> callback) { m_onPress = callback; }
            
            // Sound effects (optional)
            void SetHoverSound(void* sound) { m_hoverSound = sound; }
            void SetClickSound(void* sound) { m_clickSound = sound; }
            
            // Input handling
            void HandleEvent(const SDL_Event& event, IAudioEngine* audio = nullptr);
            
            // Rendering
            void Render(IRenderer* renderer);
            
            // Utility
            bool IsPointInside(float x, float y) const;
            
        private:
            Rect m_bounds;
            ButtonState m_state;
            bool m_enabled;
            bool m_wasHovered;
            
            std::shared_ptr<Sprite> m_normalSprite;
            std::shared_ptr<Sprite> m_hoveredSprite;
            std::shared_ptr<Sprite> m_pressedSprite;
            std::shared_ptr<Sprite> m_disabledSprite;
            
            std::function<void()> m_onClick;
            std::function<void()> m_onHover;
            std::function<void()> m_onPress;
            
            void* m_hoverSound;  // MIX_Audio* - void* to avoid header dependency
            void* m_clickSound;
            
            void UpdateState(float mouseX, float mouseY, bool mouseDown);
        };
    }
}
