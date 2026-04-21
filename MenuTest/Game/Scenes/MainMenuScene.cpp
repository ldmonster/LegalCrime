#include "MainMenuScene.h"
#include "../../Engine/Graphics/Texture.h"
#include "../../Engine/Graphics/Sprite.h"
#include "../../Engine/UI/Button.h"
#include "../../Engine/Audio/SoundEffect.h"
#include "../../Engine/Core/FileSystem.h"
#include "../GameConstants.h"
#include "../../Engine/Core/Constants.h"
#include <cmath>

namespace LegalCrime {

    MainMenuScene::MainMenuScene(Engine::ILogger* logger, Engine::IRenderer* renderer, Engine::ISoundPlayer* audio)
        : Scene("MainMenu", logger, renderer)
        , m_audio(audio)
        , m_backgroundRect(0, 0, Engine::Constants::Window::DEFAULT_WIDTH, Engine::Constants::Window::DEFAULT_HEIGHT)
        , m_shouldQuit(false)
        , m_shouldStartGame(false) {
    }

    MainMenuScene::~MainMenuScene() {
        Shutdown();
    }

    Engine::Result<void> MainMenuScene::Initialize() {
        if (m_initialized) {
            return Engine::Result<void>::Failure("MainMenuScene already initialized");
        }

        if (!m_renderer) {
            return Engine::Result<void>::Failure("Renderer is null");
        }

        if (m_logger) {
            m_logger->Info("Initializing Main Menu Scene with NEW graphics system");
        }

        // NEW: Load textures using Engine::Texture
        m_backgroundTexture = Engine::Texture::LoadFromFile(
            m_renderer,
            Engine::FileSystem::ResolveAssetPath("Pics/main_menu.png"),
            m_logger
        );

        if (!m_backgroundTexture) {
            return Engine::Result<void>::Failure("Failed to load background texture");
        }

        m_buttonsTexture = Engine::Texture::LoadFromFile(
            m_renderer,
            Engine::FileSystem::ResolveAssetPath("Pics/main_menu_buttons.png"),
            m_logger
        );

        if (!m_buttonsTexture) {
            return Engine::Result<void>::Failure("Failed to load buttons texture");
        }

        // NEW: Create background sprite
        m_backgroundSprite = std::make_shared<Engine::Sprite>(m_backgroundTexture);

        int viewWidth = Engine::Constants::Window::DEFAULT_WIDTH;
        int viewHeight = Engine::Constants::Window::DEFAULT_HEIGHT;
        if (m_renderer && m_renderer->GetNativeRenderer()) {
            int w = 0;
            int h = 0;
            if (SDL_GetCurrentRenderOutputSize(m_renderer->GetNativeRenderer(), &w, &h)) {
                if (w > 0 && h > 0) {
                    viewWidth = w;
                    viewHeight = h;
                }
            }
        }
        m_backgroundRect = Engine::Rect(0, 0, viewWidth, viewHeight);

        // NEW: Load sound effects using Engine::AudioEngine
        if (m_audio) {
            m_hoverSound = m_audio->LoadSoundEffect(Engine::FileSystem::ResolveAssetPath("Sound/OverButton.wav"));
            m_clickSound = m_audio->LoadSoundEffect(Engine::FileSystem::ResolveAssetPath("Sound/ButtonClick.wav"));

            if (!m_hoverSound) {
                m_logger->Warning("Failed to load hover sound effect");
            }
            if (!m_clickSound) {
                m_logger->Warning("Failed to load click sound effect");
            }
        }

        // Button layout constants
        using MC = LegalCrime::Constants::MainMenu;
        int xButton = 0;
        int yButton = 0;
        int widthButton = MC::BUTTON_WIDTH;
        int heightButton = MC::BUTTON_HEIGHT;
        int yHitOffset = MC::BUTTON_HIT_Y_OFFSET;

        // NEW: Create buttons using Engine::UI::Button
        for (int i = 0; i < MainMenuScene::OverallButtons; i++) {
            auto button = std::make_unique<Engine::UI::Button>();

            // Create sprites for hover state
            Engine::Rect sourceRectHover(xButton, yButton, widthButton, heightButton);
            auto hoverSprite = std::make_shared<Engine::Sprite>(
                m_buttonsTexture,
                sourceRectHover
            );

            // Create sprites for pressed state
            Engine::Rect sourceRectPressed(xButton, yButton + yHitOffset, widthButton, heightButton);
            auto pressedSprite = std::make_shared<Engine::Sprite>(
                m_buttonsTexture,
                sourceRectPressed
            );

            // Set button bounds
            ButtonLayout layout = ComputeButtonLayout(viewWidth, viewHeight, i);
            Engine::Rect bounds(layout.x, layout.y, layout.w, layout.h);
            button->SetBounds(bounds);

            // Set button sprites
            button->SetHoveredSprite(hoverSprite);
            button->SetPressedSprite(pressedSprite);

            // NEW: Set up sound effect callbacks
            button->SetOnHover([this]() {
                if (m_audio && m_hoverSound) {
                    m_audio->PlaySoundEffect(m_hoverSound);
                }
            });

            // NEW: Set up click callbacks using lambdas
            if (i == MainMenuScene::SingleplayerButton) {
                button->SetOnClick([this]() {
                    if (m_audio && m_clickSound) {
                        m_audio->PlaySoundEffect(m_clickSound);
                    }
                    m_shouldStartGame = true;
                    if (m_logger) {
                        m_logger->Info("Single player clicked");
                    }
                });
            } else if (i == MainMenuScene::ExitButton) {
                button->SetOnClick([this]() {
                    if (m_audio && m_clickSound) {
                        m_audio->PlaySoundEffect(m_clickSound);
                    }
                    m_shouldQuit = true;
                    if (m_logger) {
                        m_logger->Info("Exit clicked");
                    }
                });
            } else {
                // Default click handler for other buttons
                button->SetOnClick([this]() {
                    if (m_audio && m_clickSound) {
                        m_audio->PlaySoundEffect(m_clickSound);
                    }
                });
            }

            m_buttons[i] = std::move(button);

            yButton += heightButton;
        }

        m_initialized = true;

        if (m_logger) {
            m_logger->Info("Main Menu Scene initialized successfully with modern components!");
        }

        return Engine::Result<void>::Success();
    }

    void MainMenuScene::Shutdown() {
        if (!m_initialized) {
            return;
        }

        if (m_logger) {
            m_logger->Info("Shutting down Main Menu Scene");
        }

        for (int i = 0; i < OverallButtons; i++) {
            m_buttons[i].reset();
        }
        m_backgroundSprite.reset();
        m_backgroundTexture.reset();
        m_buttonsTexture.reset();
        m_hoverSound.reset();
        m_clickSound.reset();
        m_initialized = false;
    }

    void MainMenuScene::HandleEvent(const SDL_Event& event) {
        if (!m_initialized) {
            return;
        }

        // Only process mouse events
        if (event.type == SDL_EVENT_MOUSE_MOTION ||
            event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
            event.type == SDL_EVENT_MOUSE_BUTTON_UP) {

            // Build MouseState snapshot from SDL event (DIP: Button doesn't touch SDL)
            float mx, my;
            uint32_t buttons = SDL_GetMouseState(&mx, &my);
            Engine::Input::MouseState mouse(
                mx, my,
                (buttons & SDL_BUTTON_LMASK) != 0,
                (buttons & SDL_BUTTON_RMASK) != 0,
                (buttons & SDL_BUTTON_MMASK) != 0
            );

            for (int i = 0; i < MainMenuScene::OverallButtons; i++) {
                if (m_buttons[i]) {
                    m_buttons[i]->HandleInput(mouse);
                }
            }
        }
    }

    void MainMenuScene::Update(float deltaTime) {
        // Main menu doesn't need per-frame updates currently
    }

    void MainMenuScene::Render() {
        if (!m_initialized || !m_renderer) {
            return;
        }

        // NEW: Render using Engine::Sprite
        if (m_backgroundSprite) {
            m_backgroundSprite->Render(m_renderer, m_backgroundRect);
        }

        // NEW: Render buttons using Engine::UI::Button
        for (int i = 0; i < MainMenuScene::OverallButtons; i++) {
            if (m_buttons[i]) {
                m_buttons[i]->Render(m_renderer);
            }
        }
    }

    bool MainMenuScene::ShouldQuit() const {
        return m_shouldQuit;
    }

    bool MainMenuScene::ShouldStartGame() const {
        return m_shouldStartGame;
    }

    MainMenuScene::ButtonLayout MainMenuScene::ComputeButtonLayout(int viewWidth, int viewHeight, int buttonIndex) {
        using MC = LegalCrime::Constants::MainMenu;

        const float baseWidth = static_cast<float>(Engine::Constants::Window::DEFAULT_WIDTH);
        const float baseHeight = static_cast<float>(Engine::Constants::Window::DEFAULT_HEIGHT);
        float scaleX = (baseWidth > 0.0f) ? static_cast<float>(viewWidth) / baseWidth : 1.0f;
        float scaleY = (baseHeight > 0.0f) ? static_cast<float>(viewHeight) / baseHeight : 1.0f;

        int x = static_cast<int>(std::round(MC::BUTTON_RENDER_X * scaleX));
        int y = static_cast<int>(std::round((MC::BUTTON_RENDER_Y + buttonIndex * (MC::BUTTON_RENDER_HEIGHT + MC::BUTTON_RENDER_Y_OFFSET)) * scaleY));
        int w = static_cast<int>(std::round(MC::BUTTON_RENDER_WIDTH * scaleX));
        int h = static_cast<int>(std::round(MC::BUTTON_RENDER_HEIGHT * scaleY));

        if (buttonIndex == ExitButton) {
            y -= static_cast<int>(std::round(MC::EXIT_BUTTON_Y_ADJUST * scaleY));
        }

        if (w < 1) w = 1;
        if (h < 1) h = 1;

        return ButtonLayout{x, y, w, h};
    }
}
