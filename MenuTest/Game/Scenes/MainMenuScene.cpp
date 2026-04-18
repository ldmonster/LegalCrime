#include "MainMenuScene.h"
#include "../../Engine/Graphics/Texture.h"
#include "../../Engine/Graphics/Sprite.h"
#include "../../Engine/UI/Button.h"
#include "../../Engine/Audio/SoundEffect.h"

namespace LegalCrime {

    MainMenuScene::MainMenuScene(Engine::ILogger* logger, Engine::IRenderer* renderer, Engine::ISoundPlayer* audio)
        : Scene("MainMenu", logger, renderer)
        , m_audio(audio)
        , m_backgroundRect(0, 0, 800, 600)
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
            "./Pics/main_menu.png",
            m_logger
        );

        if (!m_backgroundTexture) {
            return Engine::Result<void>::Failure("Failed to load background texture");
        }

        m_buttonsTexture = Engine::Texture::LoadFromFile(
            m_renderer,
            "./Pics/main_menu_buttons.png",
            m_logger
        );

        if (!m_buttonsTexture) {
            return Engine::Result<void>::Failure("Failed to load buttons texture");
        }

        // NEW: Create background sprite
        m_backgroundSprite = std::make_shared<Engine::Sprite>(m_backgroundTexture);

        // NEW: Load sound effects using Engine::AudioEngine
        if (m_audio) {
            m_hoverSound = m_audio->LoadSoundEffect("Sound/OverButton.wav");
            m_clickSound = m_audio->LoadSoundEffect("Sound/ButtonClick.wav");

            if (!m_hoverSound) {
                m_logger->Warning("Failed to load hover sound effect");
            }
            if (!m_clickSound) {
                m_logger->Warning("Failed to load click sound effect");
            }
        }

        // Button layout constants
        int xButton = 0;
        int yButton = 0;
        int widthButton = 566;
        int heightButton = 84;
        int yHitOffset = 504;

        int xRenderButton = 257;
        int yRenderButton = 152;
        int widthRenderButton = 283;
        int heightRenderButton = 42;
        int offsetRenderButton = 26;

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

            if (i == MainMenuScene::ExitButton) {
                yRenderButton -= 2;
            }

            // Set button bounds
            Engine::Rect bounds(xRenderButton, yRenderButton, widthRenderButton, heightRenderButton);
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
            yRenderButton += heightRenderButton + offsetRenderButton;
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

        // NEW: Just pass event to buttons - they handle themselves!
        for (int i = 0; i < MainMenuScene::OverallButtons; i++) {
            if (m_buttons[i]) {
                m_buttons[i]->HandleEvent(event);
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
}
