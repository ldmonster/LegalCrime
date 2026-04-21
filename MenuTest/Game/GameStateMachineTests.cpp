#include "GameStateMachine.h"
#include "../Tests/SimpleTest.h"

TEST_CASE(GameStateMachine_OpeningToMainMenu) {
    LegalCrime::GameStateMachine sm(LegalCrime::GameState::Opening);
    ASSERT_TRUE(sm.Transition(LegalCrime::GameEvent::BootCompleted));
    ASSERT_TRUE(sm.GetState() == LegalCrime::GameState::MainMenu);
    return {"GameStateMachine_OpeningToMainMenu", true, ""};
}

TEST_CASE(GameStateMachine_MainMenuToPlaying) {
    LegalCrime::GameStateMachine sm(LegalCrime::GameState::MainMenu);
    ASSERT_TRUE(sm.Transition(LegalCrime::GameEvent::StartGame));
    ASSERT_TRUE(sm.GetState() == LegalCrime::GameState::Playing);
    return {"GameStateMachine_MainMenuToPlaying", true, ""};
}

TEST_CASE(GameStateMachine_MainMenuToShutdown) {
    LegalCrime::GameStateMachine sm(LegalCrime::GameState::MainMenu);
    ASSERT_TRUE(sm.Transition(LegalCrime::GameEvent::QuitRequested));
    ASSERT_TRUE(sm.GetState() == LegalCrime::GameState::Shutdown);
    return {"GameStateMachine_MainMenuToShutdown", true, ""};
}

TEST_CASE(GameStateMachine_InvalidTransition_NoStateChange) {
    LegalCrime::GameStateMachine sm(LegalCrime::GameState::MainMenu);
    ASSERT_FALSE(sm.Transition(LegalCrime::GameEvent::ReturnToMenu));
    ASSERT_TRUE(sm.GetState() == LegalCrime::GameState::MainMenu);
    return {"GameStateMachine_InvalidTransition_NoStateChange", true, ""};
}
