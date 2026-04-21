#include "GameStateMachine.h"

namespace LegalCrime {

    bool GameStateMachine::Transition(GameEvent event) {
        GameState next = m_state;

        switch (m_state) {
            case GameState::Opening:
                if (event == GameEvent::BootCompleted) {
                    next = GameState::MainMenu;
                }
                break;

            case GameState::MainMenu:
                if (event == GameEvent::StartGame) {
                    next = GameState::Playing;
                } else if (event == GameEvent::QuitRequested) {
                    next = GameState::Shutdown;
                }
                break;

            case GameState::Playing:
                if (event == GameEvent::ReturnToMenu) {
                    next = GameState::MainMenu;
                } else if (event == GameEvent::QuitRequested) {
                    next = GameState::Shutdown;
                }
                break;

            case GameState::Shutdown:
                // Terminal state
                break;
        }

        if (next == m_state) {
            return false;
        }

        m_state = next;
        return true;
    }

} // namespace LegalCrime
