#pragma once

#include "../Engine/Core/Application.h"
#include "../Engine/Core/Types.h"
#include "GameStateMachine.h"
#include <memory>

// Forward declarations for scenes
namespace LegalCrime {
    class MainMenuScene;
    class GameplayScene;
}

namespace LegalCrime {

    // LegalCrime game application
    class LegalCrimeApp : public Engine::Application {
    public:
        LegalCrimeApp();
        ~LegalCrimeApp() override;

        // Get current game state
        GameState GetGameState() const { return m_stateMachine.GetState(); }
        void SetGameState(GameState state) { m_stateMachine.SetState(state); }

    protected:
        Engine::Result<void> OnInitialize() override;
        void OnShutdown() override;
        void OnUpdate(float deltaTime) override;

    private:
        GameStateMachine m_stateMachine;

        void LoadGameScenes();
        void HandleSceneTransitions();
    };
}
