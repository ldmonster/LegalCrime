#pragma once

#include "../Engine/Core/Application.h"
#include "../Engine/Core/Types.h"
#include <memory>

// Forward declarations for scenes
namespace LegalCrime {
    class MainMenuScene;
    class GameplayScene;
}

namespace LegalCrime {

    enum class GameState {
        Opening,
        MainMenu,
        Playing,
        Shutdown
    };

    // LegalCrime game application
    class LegalCrimeApp : public Engine::Application {
    public:
        LegalCrimeApp();
        ~LegalCrimeApp() override;

        // Get current game state
        GameState GetGameState() const { return m_gameState; }
        void SetGameState(GameState state) { m_gameState = state; }

    protected:
        Engine::Result<void> OnInitialize() override;
        void OnShutdown() override;
        void OnUpdate(float deltaTime) override;

    private:
        GameState m_gameState;

        void LoadGameScenes();
        void HandleSceneTransitions();
    };
}
