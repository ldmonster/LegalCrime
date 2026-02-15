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
            
            // Character actions (for keyboard movement)
            constexpr const char* MOVE_UP = "MoveUp";
            constexpr const char* MOVE_DOWN = "MoveDown";
            constexpr const char* MOVE_LEFT = "MoveLeft";
            constexpr const char* MOVE_RIGHT = "MoveRight";
        }

        // Axis names
        namespace Axes {
            constexpr const char* MOVE_HORIZONTAL = "MoveHorizontal";
            constexpr const char* MOVE_VERTICAL = "MoveVertical";
            constexpr const char* CAMERA_PAN_X = "CameraPanX";
            constexpr const char* CAMERA_PAN_Y = "CameraPanY";
        }

        /// <summary>
        /// Initialize all game input bindings.
        /// Call this during game initialization.
        /// </summary>
        inline void InitializeBindings(Engine::Input::InputManager* inputManager) {
            if (!inputManager) return;

            // ===== ACTIONS =====

            // Select (left mouse button)
            auto* selectAction = inputManager->CreateAction(Actions::SELECT);
            selectAction->BindMouseButton(Engine::Input::MouseButton::Left);

            // Command (right mouse button)
            auto* commandAction = inputManager->CreateAction(Actions::COMMAND);
            commandAction->BindMouseButton(Engine::Input::MouseButton::Right);

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

            // ===== AXES =====

            // Movement axes (WASD / Arrow keys)
            auto* moveHorizontalAxis = inputManager->CreateAxis(Axes::MOVE_HORIZONTAL);
            moveHorizontalAxis->BindKeys(SDLK_A, SDLK_D); // Primary: A/D
            moveHorizontalAxis->SetSensitivity(1.0f);

            auto* moveVerticalAxis = inputManager->CreateAxis(Axes::MOVE_VERTICAL);
            moveVerticalAxis->BindKeys(SDLK_W, SDLK_S); // Primary: W/S
            moveVerticalAxis->SetSensitivity(1.0f);

            // Camera pan axes (for future camera drag)
            auto* cameraPanXAxis = inputManager->CreateAxis(Axes::CAMERA_PAN_X);
            cameraPanXAxis->BindMouseAxis(true); // X axis
            cameraPanXAxis->SetSensitivity(1.0f);

            auto* cameraPanYAxis = inputManager->CreateAxis(Axes::CAMERA_PAN_Y);
            cameraPanYAxis->BindMouseAxis(false); // Y axis
            cameraPanYAxis->SetSensitivity(1.0f);
        }

        /// <summary>
        /// Update input bindings (for remappable controls in the future).
        /// </summary>
        inline void UpdateBindings(Engine::Input::InputManager* inputManager) {
            // TODO: Load bindings from config file
            // For now, just use defaults
        }

    } // namespace InputBindings

} // namespace LegalCrime
