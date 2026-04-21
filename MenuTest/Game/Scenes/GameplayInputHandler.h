#pragma once

#include "../../Engine/Core/Types.h"

namespace Engine {
    class ILogger;
    class TileMap;
    class Camera2D;
    namespace Input {
        class InputManager;
    }
}

namespace LegalCrime {
namespace Simulation {
    class GameSimulation;
}

    // Handles gameplay input translation (camera pan, keyboard move, unit commands).
    class GameplayInputHandler {
    public:
        GameplayInputHandler(Engine::ILogger* logger, Engine::Input::InputManager* inputManager);

        void HandleInputActions(Simulation::GameSimulation* simulation, Engine::TileMap* tileMap, Engine::Camera2D* camera);
        void SelectCharacterAt(Simulation::GameSimulation* simulation, int screenX, int screenY, Engine::TileMap* tileMap, Engine::Camera2D* camera);
        void CommandMoveToPosition(Simulation::GameSimulation* simulation, int screenX, int screenY, Engine::TileMap* tileMap, Engine::Camera2D* camera);

    private:
        Engine::ILogger* m_logger;
        Engine::Input::InputManager* m_inputManager;
    };

} // namespace LegalCrime
