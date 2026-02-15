#include "CharacterSpriteLoader.h"

namespace Engine {

    CharacterSpriteLoader::CharacterSpriteLoader(IRenderer* renderer, ILogger* logger)
        : m_renderer(renderer)
        , m_logger(logger) {
    }

    CharacterSpriteLoader::~CharacterSpriteLoader() {
        ClearConfigs();
    }

    void CharacterSpriteLoader::RegisterConfig(const CharacterSpriteConfig& config) {
        if (config.characterType.empty()) {
            if (m_logger) {
                m_logger->Warning("CharacterSpriteLoader: Cannot register config with empty character type");
            }
            return;
        }

        m_configs[config.characterType] = config;

        if (m_logger) {
            m_logger->Info("CharacterSpriteLoader: Registered config for '" + config.characterType + 
                         "' with " + std::to_string(config.animations.size()) + " animations");
        }
    }

    std::unique_ptr<CharacterSprite> CharacterSpriteLoader::CreateCharacter(const std::string& characterType) {
        // Check if config exists
        auto configIt = m_configs.find(characterType);
        if (configIt == m_configs.end()) {
            if (m_logger) {
                m_logger->Error("CharacterSpriteLoader: No config found for character type '" + 
                              characterType + "'");
            }
            return nullptr;
        }

        const CharacterSpriteConfig& config = configIt->second;

        // Load or get cached texture
        std::shared_ptr<Texture> texture;
        auto textureIt = m_textureCache.find(config.spriteSheetPath);
        
        if (textureIt != m_textureCache.end()) {
            // Use cached texture
            texture = textureIt->second;
            if (m_logger) {
                m_logger->Debug("CharacterSpriteLoader: Using cached texture for '" + 
                              config.spriteSheetPath + "'");
            }
        } else {
            // Load new texture
            texture = Texture::LoadFromFile(m_renderer, config.spriteSheetPath, m_logger);
            
            if (!texture) {
                if (m_logger) {
                    m_logger->Error("CharacterSpriteLoader: Failed to load texture from '" + 
                                  config.spriteSheetPath + "'");
                }
                return nullptr;
            }

            // Cache the texture for future use
            m_textureCache[config.spriteSheetPath] = texture;
            
            if (m_logger) {
                m_logger->Info("CharacterSpriteLoader: Loaded and cached texture '" + 
                             config.spriteSheetPath + "'");
            }
        }

        // Create character sprite
        auto character = std::make_unique<CharacterSprite>(
            characterType,
            texture,
            config.defaultScale,
            m_logger
        );

        // Add all animations from config
        for (const auto& animConfig : config.animations) {
            character->AddAnimation(animConfig);
        }

        // Set default animation
        if (!config.defaultAnimation.empty()) {
            character->SetAnimation(config.defaultAnimation);
        }

        if (m_logger) {
            m_logger->Info("CharacterSpriteLoader: Created character '" + characterType + 
                         "' with " + std::to_string(config.animations.size()) + " animations");
        }

        return character;
    }

    bool CharacterSpriteLoader::HasConfig(const std::string& characterType) const {
        return m_configs.find(characterType) != m_configs.end();
    }

    void CharacterSpriteLoader::ClearConfigs() {
        m_configs.clear();
        m_textureCache.clear();
    }

} // namespace Engine
