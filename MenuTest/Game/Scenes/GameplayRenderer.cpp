#include "GameplayRenderer.h"
#include "../Simulation/GameSimulation.h"
#include "../World/World.h"
#include "../World/Systems/SelectionSystem.h"
#include "../Entities/Character.h"
#include "../../Engine/Renderer/IRenderer.h"
#include "../../Engine/World/TileMap.h"
#include "../../Engine/World/TileMapRenderer.h"
#include "../../Engine/Camera/Camera2D.h"
#include "../../Engine/Core/Logger/ILogger.h"

namespace LegalCrime {

    GameplayRenderer::GameplayRenderer(Engine::ILogger* logger, Engine::IRenderer* renderer)
        : m_logger(logger)
        , m_renderer(renderer) {
    }

    void GameplayRenderer::SyncEntityScreenPositions(
        Simulation::GameSimulation* simulation,
        Engine::TileMap* tileMap,
        Engine::Camera2D* camera) {

        if (!simulation || !tileMap) {
            return;
        }

        auto* world = simulation->GetWorld();
        if (!world) {
            return;
        }

        for (auto* character : world->GetAllCharacters()) {
            if (!character) {
                continue;
            }

            Engine::TilePosition charPos = character->GetTilePosition();
            Engine::Point screenPos = tileMap->TileToScreenCenter(charPos.row, charPos.col, camera);

            int spriteWidth = static_cast<int>(50 * character->GetScale());
            int spriteHeight = static_cast<int>(50 * character->GetScale());
            screenPos.x -= spriteWidth / 2;
            screenPos.y -= spriteHeight / 2;

            int tileHeightOffset = tileMap->GetTileHeight();
            screenPos.y -= tileHeightOffset;

            character->SetPosition(screenPos.x, screenPos.y);
        }
    }

    void GameplayRenderer::Render(
        Simulation::GameSimulation* simulation,
        Engine::TileMap* tileMap,
        Engine::TileMapRenderer* tileMapRenderer,
        Engine::Camera2D* camera) {

        if (!simulation || !tileMap || !tileMapRenderer || !m_renderer) {
            return;
        }

        tileMapRenderer->Render(m_renderer, *tileMap, camera);

        auto* world = simulation->GetWorld();
        if (world) {
            for (auto* character : world->GetAllCharacters()) {
                if (character) {
                    character->Render(m_renderer);
                }
            }
        }

        auto* selectionSystem = simulation->GetSelectionSystem();
        if (!selectionSystem || !selectionSystem->HasSelection()) {
            return;
        }

        for (auto* selectedChar : selectionSystem->GetSelectedCharacters()) {
            if (!selectedChar) {
                continue;
            }

            Engine::Point tileCenter = tileMap->TileToScreenCenter(
                selectedChar->GetTilePosition().row,
                selectedChar->GetTilePosition().col,
                camera
            );

            m_shapeCache.DrawCircle(*m_renderer, tileCenter, 40, Engine::Color(0, 255, 0, 255));
        }

        if (selectionSystem->IsBoxSelecting()) {
            Engine::Rect box = selectionSystem->GetBoxSelectRect();
            m_renderer->SetDrawColor(0, 255, 0, 128);
            Engine::Point boxPoints[5] = {
                {box.x, box.y},
                {box.x + box.w, box.y},
                {box.x + box.w, box.y + box.h},
                {box.x, box.y + box.h},
                {box.x, box.y}
            };
            m_renderer->DrawLines(boxPoints, 5);
        }
    }

} // namespace LegalCrime
