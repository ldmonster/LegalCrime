#pragma once

#include "../../Engine/Scene/Scene.h"
#include "../../Engine/Core/Logger/ILogger.h"
#include "../../Engine/Renderer/IRenderer.h"
#include "../../Engine/Audio/IAudioEngine.h"
#include <SDL3/SDL.h>
#include <memory>

namespace LegalCrime {

    // Forward declaration for implementation
    class MainMenuSceneImpl;

    // Main menu scene
    class MainMenuScene : public Engine::Scene {
    public:
        MainMenuScene(Engine::ILogger* logger, Engine::IRenderer* renderer, Engine::IAudioEngine* audio);
        ~MainMenuScene() override;

        Engine::Result<void> Initialize() override;
        void Shutdown() override;

        void HandleEvent(const SDL_Event& event) override;
        void Update(float deltaTime) override;
        void Render() override;

        // Check if user wants to quit or transition to gameplay
        bool ShouldQuit() const;
        bool ShouldStartGame() const;

    private:
        std::unique_ptr<MainMenuSceneImpl> m_impl;
    };
}
