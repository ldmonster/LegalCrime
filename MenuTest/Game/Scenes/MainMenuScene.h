#pragma once

#include "../../Engine/Scene/Scene.h"
#include "../../Engine/Core/Types.h"
#include "../../Engine/Core/Logger/ILogger.h"
#include "../../Engine/Renderer/IRenderer.h"
#include "../../Engine/Audio/ISoundPlayer.h"
#include "../../Engine/Input/InputTypes.h"
#include <SDL3/SDL.h>
#include <memory>

// Forward declarations
namespace Engine {
    class SoundEffect;
    class Texture;
    class Sprite;
    namespace UI { class Button; }
}

namespace LegalCrime {

    // Main menu scene
    class MainMenuScene : public Engine::Scene {
    public:
        MainMenuScene(Engine::ILogger* logger, Engine::IRenderer* renderer, Engine::ISoundPlayer* audio);
        ~MainMenuScene() override;

        Engine::Result<void> Initialize() override;
        void Shutdown() override;

        void HandleEvent(const SDL_Event& event) override;
        void Update(float deltaTime) override;
        void Render() override;

        Engine::SceneId GetSceneId() const override { return Engine::SceneId::MainMenu; }

        // Check if user wants to quit or transition to gameplay
        bool ShouldQuit() const;
        bool ShouldStartGame() const;

    private:
        enum MainPageButtons {
            MultiplayerButton,
            SingleplayerButton,
            SettingsButton,
            CreditsButton,
            CultureButton,
            ExitButton,
            OverallButtons
        };

        Engine::ISoundPlayer* m_audio;

        std::shared_ptr<Engine::SoundEffect> m_hoverSound;
        std::shared_ptr<Engine::SoundEffect> m_clickSound;

        std::shared_ptr<Engine::Texture> m_backgroundTexture;
        std::shared_ptr<Engine::Texture> m_buttonsTexture;

        std::shared_ptr<Engine::Sprite> m_backgroundSprite;

        std::unique_ptr<Engine::UI::Button> m_buttons[OverallButtons];

        Engine::Rect m_backgroundRect;

        bool m_shouldQuit;
        bool m_shouldStartGame;
    };
}
