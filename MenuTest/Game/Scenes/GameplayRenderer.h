#pragma once

#include "../../Engine/Renderer/ShapeCache.h"

namespace Engine {
    class ILogger;
    class IRenderer;
    class TileMap;
    class TileMapRenderer;
    class Camera2D;
}

namespace LegalCrime {
namespace Simulation {
    class GameSimulation;
}

    // Renders gameplay presentation from simulation state.
    class GameplayRenderer {
    public:
        GameplayRenderer(Engine::ILogger* logger, Engine::IRenderer* renderer);

        void SyncEntityScreenPositions(Simulation::GameSimulation* simulation, Engine::TileMap* tileMap, Engine::Camera2D* camera);
        void Render(Simulation::GameSimulation* simulation, Engine::TileMap* tileMap, Engine::TileMapRenderer* tileMapRenderer, Engine::Camera2D* camera);

    private:
        Engine::ILogger* m_logger;
        Engine::IRenderer* m_renderer;
        Engine::ShapeCache m_shapeCache;
    };

} // namespace LegalCrime
