#pragma once

#include "../../Engine/Scene/Scene.h"
#include "../../Engine/Core/Logger/ILogger.h"
#include "../../Engine/Renderer/IRenderer.h"
#include <SDL3/SDL.h>
#include <memory>

namespace LegalCrime {

    // Forward declaration for implementation
    class GameplaySceneImpl;

    // Gameplay scene
    class GameplayScene : public Engine::Scene {
    public:
        GameplayScene(Engine::ILogger* logger, Engine::IRenderer* renderer);
        ~GameplayScene() override;

        Engine::Result<void> Initialize() override;
        void Shutdown() override;

        void HandleEvent(const SDL_Event& event) override;
        void Update(float deltaTime) override;
        void Render() override;

        // Character movement
        void MoveCharacterToTile(uint16_t row, uint16_t col, float duration = 0.3f);

    private:
        std::unique_ptr<GameplaySceneImpl> m_impl;
    };
}
