#include "Button.h"
#include "../Renderer/IRenderer.h"

namespace Engine {
    namespace UI {
        
        Button::Button()
            : m_bounds()
            , m_state(ButtonState::Normal)
            , m_enabled(true)
            , m_wasHovered(false)
            , m_wasPressed(false)
            , m_normalSprite(nullptr)
            , m_hoveredSprite(nullptr)
            , m_pressedSprite(nullptr)
            , m_disabledSprite(nullptr)
            , m_onClick(nullptr)
            , m_onHover(nullptr)
            , m_onPress(nullptr) {
        }
        
        void Button::HandleInput(const Input::MouseState& mouse) {
            if (!m_enabled) {
                m_state = ButtonState::Disabled;
                return;
            }
            
            bool isInside = IsPointInside(mouse.x, mouse.y);
            ButtonState oldState = m_state;
            
            if (!isInside) {
                m_state = ButtonState::Normal;
                m_wasHovered = false;
                m_wasPressed = false;
            } else {
                if (mouse.leftDown) {
                    m_state = ButtonState::Pressed;
                    
                    // Trigger press callback on transition
                    if (!m_wasPressed && m_onPress) {
                        m_onPress();
                    }
                    m_wasPressed = true;
                } else {
                    m_state = ButtonState::Hovered;
                    
                    // Trigger hover callback (only once when entering)
                    if (!m_wasHovered && m_onHover) {
                        m_onHover();
                    }
                    m_wasHovered = true;
                    
                    // If we were pressed and released mouse inside button, trigger click
                    if (m_wasPressed && m_onClick) {
                        m_onClick();
                    }
                    m_wasPressed = false;
                }
            }
        }
        
        void Button::Render(IRenderer* renderer) {
            if (!renderer) return;
            
            std::shared_ptr<Sprite> spriteToRender;
            
            switch (m_state) {
                case ButtonState::Disabled:
                    spriteToRender = m_disabledSprite ? m_disabledSprite : m_normalSprite;
                    break;
                case ButtonState::Pressed:
                    spriteToRender = m_pressedSprite ? m_pressedSprite : m_normalSprite;
                    break;
                case ButtonState::Hovered:
                    spriteToRender = m_hoveredSprite ? m_hoveredSprite : m_normalSprite;
                    break;
                case ButtonState::Normal:
                default:
                    spriteToRender = m_normalSprite;
                    break;
            }
            
            if (spriteToRender) {
                spriteToRender->Render(renderer, m_bounds);
            }
        }
        
        bool Button::IsPointInside(float x, float y) const {
            return x >= m_bounds.x && x < m_bounds.x + m_bounds.w &&
                   y >= m_bounds.y && y < m_bounds.y + m_bounds.h;
        }
    }
}
