#include "GameResourceLoader.h"
#include "../../Engine/Core/Logger/ILogger.h"

namespace LegalCrime {
namespace Resources {

    void GameResourceLoader::LoadEssentialResources(Engine::Resources::ResourceManager* resourceManager) {
        if (!resourceManager) return;

        // Load the most commonly used resources
        LoadCharacterResources(resourceManager);
        RegisterMusicResources(resourceManager);
    }

    void GameResourceLoader::LoadMainMenuResources(Engine::Resources::ResourceManager* resourceManager) {
        if (!resourceManager) return;

        // Load UI resources for main menu
        LoadUIResources(resourceManager);
        
        // Register main menu music
        resourceManager->RegisterMusic(
            ResourceIDs::GetMusicName(MusicID::MainMenu),
            Paths::Music::MAIN_MENU
        );
    }

    void GameResourceLoader::LoadGameplayResources(Engine::Resources::ResourceManager* resourceManager) {
        if (!resourceManager) return;

        // Load character resources
        LoadCharacterResources(resourceManager);
        
        // Load sound effects
        LoadSoundResources(resourceManager);
        
        // Register gameplay music
        resourceManager->RegisterMusic(
            ResourceIDs::GetMusicName(MusicID::Gameplay),
            Paths::Music::GAMEPLAY
        );
        
        resourceManager->RegisterMusic(
            ResourceIDs::GetMusicName(MusicID::Combat),
            Paths::Music::COMBAT
        );
    }

    void GameResourceLoader::LoadCharacterResources(Engine::Resources::ResourceManager* resourceManager) {
        if (!resourceManager) return;

        // Preload character spritesheets
        std::vector<std::pair<std::string, std::string>> characterTextures = {
            { ResourceIDs::GetTextureName(TextureID::ThugSpritesheet), Paths::Textures::THUG_SPRITESHEET }
            // Add more character spritesheets as they become available:
            // { ResourceIDs::GetTextureName(TextureID::CopSpritesheet), Paths::Textures::COP_SPRITESHEET },
            // { ResourceIDs::GetTextureName(TextureID::CivilianSpritesheet), Paths::Textures::CIVILIAN_SPRITESHEET }
        };

        resourceManager->PreloadTextures(characterTextures);
    }

    void GameResourceLoader::LoadUIResources(Engine::Resources::ResourceManager* resourceManager) {
        if (!resourceManager) return;
        // UI textures loaded on-demand when actual assets exist.
    }

    void GameResourceLoader::LoadSoundResources(Engine::Resources::ResourceManager* resourceManager) {
        if (!resourceManager) return;
        // Sound effects loaded on-demand when actual assets exist.
    }

    void GameResourceLoader::RegisterMusicResources(Engine::Resources::ResourceManager* resourceManager) {
        if (!resourceManager) return;

        // Register music tracks (paths only, not loaded into memory)
        // Music is streamed, so we just register the paths
        
        // Note: Update these when actual music files are organized
        resourceManager->RegisterMusic(
            ResourceIDs::GetMusicName(MusicID::MainMenu),
            Paths::Music::MAIN_MENU
        );
        
        resourceManager->RegisterMusic(
            ResourceIDs::GetMusicName(MusicID::Gameplay),
            Paths::Music::GAMEPLAY
        );
        
        resourceManager->RegisterMusic(
            ResourceIDs::GetMusicName(MusicID::Combat),
            Paths::Music::COMBAT
        );
    }

} // namespace Resources
} // namespace LegalCrime
