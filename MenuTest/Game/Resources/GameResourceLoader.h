#pragma once

#include "../../Engine/Resources/ResourceManager.h"
#include "ResourcePaths.h"
#include "GameResources.h"

namespace LegalCrime {
namespace Resources {

    /// <summary>
    /// Helper class to load all game resources.
    /// Call during game initialization to preload commonly used assets.
    /// </summary>
    class GameResourceLoader {
    public:
        /// <summary>
        /// Load all essential resources (textures, sounds, music paths).
        /// </summary>
        static void LoadEssentialResources(Engine::Resources::ResourceManager* resourceManager);

        /// <summary>
        /// Load resources needed for the main menu.
        /// </summary>
        static void LoadMainMenuResources(Engine::Resources::ResourceManager* resourceManager);

        /// <summary>
        /// Load resources needed for gameplay.
        /// </summary>
        static void LoadGameplayResources(Engine::Resources::ResourceManager* resourceManager);

        /// <summary>
        /// Preload all character spritesheets.
        /// </summary>
        static void LoadCharacterResources(Engine::Resources::ResourceManager* resourceManager);

        /// <summary>
        /// Preload all UI resources.
        /// </summary>
        static void LoadUIResources(Engine::Resources::ResourceManager* resourceManager);

        /// <summary>
        /// Preload all sound effects.
        /// </summary>
        static void LoadSoundResources(Engine::Resources::ResourceManager* resourceManager);

        /// <summary>
        /// Register all music tracks (doesn't load, just registers paths).
        /// </summary>
        static void RegisterMusicResources(Engine::Resources::ResourceManager* resourceManager);
    };

} // namespace Resources
} // namespace LegalCrime
