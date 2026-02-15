#include "Button.h"
#include "../Renderer/IRenderer.h"
#include "../Audio/IAudioEngine.h"
#include <SDL3_mixer/SDL_mixer.h>

namespace Engine {
    namespace UI {
        
        Button::Button()
            : m_bounds()
            , m_state(ButtonState::Normal)
            , m_enabled(true)
            , m_wasHovered(false)
            , m_normalSprite(nullptr)
            , m_hoveredSprite(nullptr)
            , m_pressedSprite(nullptr)
            , m_disabledSprite(nullptr)
            , m_onClick(nullptr)
            , m_onHover(nullptr)
            , m_onPress(nullptr)
            , m_hoverSound(nullptr)
            , m_clickSound(nullptr) {
        }
        
        void Button::HandleEvent(const SDL_Event& event, IAudioEngine* audio) {
            if (!m_enabled) {
                m_state = ButtonState::Disabled;
                return;
            }
            
            if (event.type == SDL_EVENT_MOUSE_MOTION ||
                event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
                event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                
                float x, y;
                SDL_GetMouseState(&x, &y);
                
                bool isInside = IsPointInside(x, y);
                bool mouseDown = (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LMASK) != 0;
                
                ButtonState oldState = m_state;
                
                if (!isInside) {
                    m_state = ButtonState::Normal;
                    m_wasHovered = false;
                } else {
                    if (mouseDown) {
                        m_state = ButtonState::Pressed;
                        
                        // Trigger press callback
                        if (oldState != ButtonState::Pressed && m_onPress) {
                            m_onPress();
                        }
                    } else {
                        m_state = ButtonState::Hovered;
                        
                        // Trigger hover callback (only once when entering)
                        if (!m_wasHovered && m_onHover) {
                            m_onHover();
                            
                            // Play hover sound
                            // TODO: Integrate with AudioEngine for sound effects
                            // For now, keeping compatibility with old system
                        }
                        m_wasHovered = true;
                        
                        // If we were pressed and released mouse inside button, trigger click
                        if (oldState == ButtonState::Pressed && 
                            event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                            if (m_onClick) {
                                m_onClick();
                            }
                            
                            // Play click sound
                            // TODO: Same as hover sound
                        }
                    }
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
