#include "MainMenuScene.h"
#include "../../Engine/Graphics/Texture.h"
#include "../../Engine/Graphics/Sprite.h"
#include "../../Engine/UI/Button.h"
#include "../../Engine/Audio/SoundEffect.h"

namespace LegalCrime {

    // Private implementation using NEW engine components
    class MainMenuSceneImpl {
    public:
        enum MainPageButtons {
            MultiplayerButton,
            SingleplayerButton,
            SettingsButton,
            CreditsButton,
            CultureButton,
            ExitButton,
            OverallButtons
        };

        Engine::IRenderer* renderer;
        Engine::IAudioEngine* audio;

        // NEW: Using Engine::SoundEffect instead of MainMenu_Sound
        std::shared_ptr<Engine::SoundEffect> hoverSound;
        std::shared_ptr<Engine::SoundEffect> clickSound;

        // NEW: Using Engine::Texture (shared_ptr for resource sharing)
        std::shared_ptr<Engine::Texture> backgroundTexture;
        std::shared_ptr<Engine::Texture> buttonsTexture;

        // NEW: Using Engine::Sprite
        std::shared_ptr<Engine::Sprite> backgroundSprite;

        // NEW: Using Engine::UI::Button
        std::unique_ptr<Engine::UI::Button> buttons[OverallButtons];

        Engine::Rect backgroundRect;

        bool shouldQuit;
        bool shouldStartGame;

        MainMenuSceneImpl()
            : renderer(nullptr)
            , audio(nullptr)
            , hoverSound(nullptr)
            , clickSound(nullptr)
            , backgroundTexture(nullptr)
            , buttonsTexture(nullptr)
            , backgroundSprite(nullptr)
            , backgroundRect(0, 0, 800, 600)
            , shouldQuit(false)
            , shouldStartGame(false) {

            for (int i = 0; i < OverallButtons; i++) {
                buttons[i] = nullptr;
            }
        }

        ~MainMenuSceneImpl() {
            Cleanup();
        }

        void Cleanup() {
            // NEW: Smart pointers clean themselves up!
            for (int i = 0; i < OverallButtons; i++) {
                buttons[i].reset();
            }

            backgroundSprite.reset();
            backgroundTexture.reset();
            buttonsTexture.reset();
            hoverSound.reset();
            clickSound.reset();
        }
    };

    MainMenuScene::MainMenuScene(Engine::ILogger* logger, Engine::IRenderer* renderer, Engine::IAudioEngine* audio)
        : Scene("MainMenu", logger, renderer)
        , m_impl(std::make_unique<MainMenuSceneImpl>()) {
        m_impl->renderer = renderer;
        m_impl->audio = audio;
    }

    MainMenuScene::~MainMenuScene() {
        Shutdown();
    }

    Engine::Result<void> MainMenuScene::Initialize() {
        if (m_initialized) {
            return Engine::Result<void>::Failure("MainMenuScene already initialized");
        }

        if (!m_impl->renderer) {
            return Engine::Result<void>::Failure("Renderer is null");
        }

        if (m_logger) {
            m_logger->Info("Initializing Main Menu Scene with NEW graphics system");
        }

        // NEW: Load textures using Engine::Texture
        m_impl->backgroundTexture = Engine::Texture::LoadFromFile(
            m_impl->renderer,
            "./Pics/main_menu.png",
            m_logger
        );

        if (!m_impl->backgroundTexture) {
            return Engine::Result<void>::Failure("Failed to load background texture");
        }

        m_impl->buttonsTexture = Engine::Texture::LoadFromFile(
            m_impl->renderer,
            "./Pics/main_menu_buttons.png",
            m_logger
        );

        if (!m_impl->buttonsTexture) {
            return Engine::Result<void>::Failure("Failed to load buttons texture");
        }

        // NEW: Create background sprite
        m_impl->backgroundSprite = std::make_shared<Engine::Sprite>(m_impl->backgroundTexture);

        // NEW: Load sound effects using Engine::AudioEngine
        if (m_impl->audio) {
            m_impl->hoverSound = m_impl->audio->LoadSoundEffect("Sound/OverButton.wav");
            m_impl->clickSound = m_impl->audio->LoadSoundEffect("Sound/ButtonClick.wav");

            if (!m_impl->hoverSound) {
                m_logger->Warning("Failed to load hover sound effect");
            }
            if (!m_impl->clickSound) {
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
        for (int i = 0; i < MainMenuSceneImpl::OverallButtons; i++) {
            auto button = std::make_unique<Engine::UI::Button>();

            // Create sprites for hover state
            Engine::Rect sourceRectHover(xButton, yButton, widthButton, heightButton);
            auto hoverSprite = std::make_shared<Engine::Sprite>(
                m_impl->buttonsTexture,
                sourceRectHover
            );

            // Create sprites for pressed state
            Engine::Rect sourceRectPressed(xButton, yButton + yHitOffset, widthButton, heightButton);
            auto pressedSprite = std::make_shared<Engine::Sprite>(
                m_impl->buttonsTexture,
                sourceRectPressed
            );

            if (i == MainMenuSceneImpl::ExitButton) {
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
                if (m_impl->audio && m_impl->hoverSound) {
                    m_impl->audio->PlaySoundEffect(m_impl->hoverSound);
                }
            });

            // NEW: Set up click callbacks using lambdas
            if (i == MainMenuSceneImpl::SingleplayerButton) {
                button->SetOnClick([this]() {
                    if (m_impl->audio && m_impl->clickSound) {
                        m_impl->audio->PlaySoundEffect(m_impl->clickSound);
                    }
                    m_impl->shouldStartGame = true;
                    if (m_logger) {
                        m_logger->Info("Single player clicked");
                    }
                });
            } else if (i == MainMenuSceneImpl::ExitButton) {
                button->SetOnClick([this]() {
                    if (m_impl->audio && m_impl->clickSound) {
                        m_impl->audio->PlaySoundEffect(m_impl->clickSound);
                    }
                    m_impl->shouldQuit = true;
                    if (m_logger) {
                        m_logger->Info("Exit clicked");
                    }
                });
            } else {
                // Default click handler for other buttons
                button->SetOnClick([this]() {
                    if (m_impl->audio && m_impl->clickSound) {
                        m_impl->audio->PlaySoundEffect(m_impl->clickSound);
                    }
                });
            }

            m_impl->buttons[i] = std::move(button);

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

        m_impl->Cleanup();
        m_initialized = false;
    }

    void MainMenuScene::HandleEvent(const SDL_Event& event) {
        if (!m_initialized) {
            return;
        }

        // NEW: Just pass event to buttons - they handle themselves!
        for (int i = 0; i < MainMenuSceneImpl::OverallButtons; i++) {
            if (m_impl->buttons[i]) {
                m_impl->buttons[i]->HandleEvent(event, m_impl->audio);
            }
        }
    }

    void MainMenuScene::Update(float deltaTime) {
        // Main menu doesn't need per-frame updates currently
    }

    void MainMenuScene::Render() {
        if (!m_initialized || !m_impl->renderer) {
            return;
        }

        // NEW: Render using Engine::Sprite
        if (m_impl->backgroundSprite) {
            m_impl->backgroundSprite->Render(m_impl->renderer, m_impl->backgroundRect);
        }

        // NEW: Render buttons using Engine::UI::Button
        for (int i = 0; i < MainMenuSceneImpl::OverallButtons; i++) {
            if (m_impl->buttons[i]) {
                m_impl->buttons[i]->Render(m_impl->renderer);
            }
        }
    }

    bool MainMenuScene::ShouldQuit() const {
        return m_impl->shouldQuit;
    }

    bool MainMenuScene::ShouldStartGame() const {
        return m_impl->shouldStartGame;
    }
}
