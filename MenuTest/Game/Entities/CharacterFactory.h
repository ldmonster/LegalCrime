#pragma once

#include "Character.h"
#include "../../Engine/Resources/ResourceManager.h"
#include "../../Engine/Core/Logger/ILogger.h"
#include <memory>
#include <string>

namespace LegalCrime {
namespace Entities {

    /// <summary>
    /// Factory for creating character entities from configurations.
    /// Replaces Engine::CharacterSpriteLoader with game-specific character creation.
    /// </summary>
    class CharacterFactory {
    public:
        CharacterFactory(
            Engine::Resources::ResourceManager* resourceManager,
            Engine::ILogger* logger = nullptr);

        ~CharacterFactory();

        /// <summary>
        /// Create a character of the specified type.
        /// Uses ResourceManager to load textures and configurations.
        /// </summary>
        std::unique_ptr<Character> CreateCharacter(CharacterType type);

        /// <summary>
        /// Create a character from a character type string (e.g., "thug", "cop").
        /// </summary>
        std::unique_ptr<Character> CreateCharacter(const std::string& typeName);

    private:
        Engine::Resources::ResourceManager* m_resourceManager;
        Engine::ILogger* m_logger;

        Engine::CharacterSpriteConfig GetConfigForType(CharacterType type);
        std::string GetTexturePathForType(CharacterType type);
        std::string GetResourceNameForType(CharacterType type);
    };

} // namespace Entities
} // namespace LegalCrime
