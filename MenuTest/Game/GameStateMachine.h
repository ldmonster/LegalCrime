#pragma once

namespace LegalCrime {

    enum class GameState {
        Opening,
        MainMenu,
        Playing,
        Shutdown
    };

    enum class GameEvent {
        BootCompleted,
        StartGame,
        ReturnToMenu,
        QuitRequested
    };

    // Pure state machine for app-level game flow.
    class GameStateMachine {
    public:
        explicit GameStateMachine(GameState initialState = GameState::Opening)
            : m_state(initialState) {}

        GameState GetState() const { return m_state; }
        void SetState(GameState state) { m_state = state; }

        bool Transition(GameEvent event);

    private:
        GameState m_state;
    };

} // namespace LegalCrime
