#pragma once

#include "../../Engine/Input/InputManager.h"
#include "../../Engine/Input/InputTypes.h"
#include <SDL3/SDL.h>

namespace LegalCrime {

    /// <summary>
    /// Game-specific input action and axis names.
    /// Centralized location for all input bindings.
    /// </summary>
    namespace InputBindings {

        // Action names
        namespace Actions {
            constexpr const char* SELECT = "Select";
            constexpr const char* COMMAND = "Command";
            constexpr const char* CANCEL = "Cancel";
            constexpr const char* PAUSE = "Pause";
            constexpr const char* CONFIRM = "Confirm";
            constexpr const char* CAMERA_PAN = "CameraPan";

            // Character actions (for keyboard movement)
            constexpr const char* MOVE_UP = "MoveUp";
            constexpr const char* MOVE_DOWN = "MoveDown";
            constexpr const char* MOVE_LEFT = "MoveLeft";
            constexpr const char* MOVE_RIGHT = "MoveRight";
        }

        /// Initialize all game input bindings.
        inline void InitializeBindings(Engine::Input::InputManager* inputManager) {
            if (!inputManager) return;

            // ===== ACTIONS =====

            // Select (left mouse button)
            auto* selectAction = inputManager->CreateAction(Actions::SELECT);
            selectAction->BindMouseButton(Engine::Input::MouseButton::Left);

            // Command (right mouse button)
            auto* commandAction = inputManager->CreateAction(Actions::COMMAND);
            commandAction->BindMouseButton(Engine::Input::MouseButton::Right);

            // Camera Pan (middle mouse button)
            auto* cameraPanAction = inputManager->CreateAction(Actions::CAMERA_PAN);
            cameraPanAction->BindMouseButton(Engine::Input::MouseButton::Middle);

            // Cancel (ESC key)
            auto* cancelAction = inputManager->CreateAction(Actions::CANCEL);
            cancelAction->BindKey(SDLK_ESCAPE);

            // Pause (ESC or P key)
            auto* pauseAction = inputManager->CreateAction(Actions::PAUSE);
            pauseAction->BindKey(SDLK_ESCAPE);
            pauseAction->BindKey(SDLK_P);

            // Confirm (Enter or Space)
            auto* confirmAction = inputManager->CreateAction(Actions::CONFIRM);
            confirmAction->BindKey(SDLK_RETURN);
            confirmAction->BindKey(SDLK_SPACE);

            // Character movement (WASD and Arrow keys)
            auto* moveUpAction = inputManager->CreateAction(Actions::MOVE_UP);
            moveUpAction->BindKey(SDLK_W);
            moveUpAction->BindKey(SDLK_UP);

            auto* moveDownAction = inputManager->CreateAction(Actions::MOVE_DOWN);
            moveDownAction->BindKey(SDLK_S);
            moveDownAction->BindKey(SDLK_DOWN);

            auto* moveLeftAction = inputManager->CreateAction(Actions::MOVE_LEFT);
            moveLeftAction->BindKey(SDLK_A);
            moveLeftAction->BindKey(SDLK_LEFT);

            auto* moveRightAction = inputManager->CreateAction(Actions::MOVE_RIGHT);
            moveRightAction->BindKey(SDLK_D);
            moveRightAction->BindKey(SDLK_RIGHT);
        }

    } // namespace InputBindings

} // namespace LegalCrime
