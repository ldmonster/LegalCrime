#pragma once

#include "CharacterSprite.h"
#include "CharacterSpriteConfig.h"
#include "../Core/Logger/ILogger.h"
#include "../Renderer/IRenderer.h"
#include <memory>
#include <unordered_map>
#include <string>

namespace Engine {

    /// <summary>
    /// Factory class for loading and creating character sprites.
    /// Manages sprite configurations and creates CharacterSprite instances.
    /// </summary>
    class CharacterSpriteLoader {
    public:
        CharacterSpriteLoader(IRenderer* renderer, ILogger* logger = nullptr);
        ~CharacterSpriteLoader();

        // Register a character configuration
        void RegisterConfig(const CharacterSpriteConfig& config);

        // Create a character sprite from a registered configuration
        std::unique_ptr<CharacterSprite> CreateCharacter(const std::string& characterType);

        // Check if a character type is registered
        bool HasConfig(const std::string& characterType) const;

        // Clear all registered configurations
        void ClearConfigs();

    private:
        IRenderer* m_renderer;
        ILogger* m_logger;
        std::unordered_map<std::string, CharacterSpriteConfig> m_configs;
        std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureCache;
    };

} // namespace Engine
