#include "CharacterFactory.h"
#include "../CharacterConfigs.h"
#include "../Resources/ResourcePaths.h"
#include "../Resources/GameResources.h"

namespace LegalCrime {
namespace Entities {

    CharacterFactory::CharacterFactory(
        Engine::Resources::ResourceManager* resourceManager,
        Engine::ILogger* logger)
        : m_resourceManager(resourceManager)
        , m_logger(logger) {

        if (!m_resourceManager) {
            if (m_logger) {
                m_logger->Error("CharacterFactory: ResourceManager is null");
            }
        }
    }

    CharacterFactory::~CharacterFactory() {
    }

    std::unique_ptr<Character> CharacterFactory::CreateCharacter(CharacterType type) {
        if (!m_resourceManager) {
            if (m_logger) {
                m_logger->Error("CharacterFactory: Cannot create character without ResourceManager");
            }
            return nullptr;
        }

        // Get configuration for character type
        Engine::CharacterSpriteConfig config = GetConfigForType(type);
        if (config.characterType.empty()) {
            if (m_logger) {
                m_logger->Error("CharacterFactory: Failed to get config for character type " + 
                              std::to_string(static_cast<int>(type)));
            }
            return nullptr;
        }

        // Get texture from resource manager
        std::string resourceName = GetResourceNameForType(type);
        auto texture = m_resourceManager->GetTexture(resourceName);

        if (!texture) {
            // Try to load if not already loaded
            std::string texturePath = GetTexturePathForType(type);
            texture = m_resourceManager->LoadTexture(resourceName, texturePath);

            if (!texture) {
                if (m_logger) {
                    m_logger->Error("CharacterFactory: Failed to load texture for " + 
                                  CharacterDataUtils::CharacterTypeToString(type));
                }
                return nullptr;
            }
        }

        // Create character
        auto character = std::make_unique<Character>(type, texture, config, m_logger);

        if (m_logger) {
            m_logger->Info("CharacterFactory: Created character '" + 
                         CharacterDataUtils::CharacterTypeToString(type) + "'");
        }

        return character;
    }

    std::unique_ptr<Character> CharacterFactory::CreateCharacter(const std::string& typeName) {
        // Convert string to CharacterType enum
        CharacterType type = CharacterType::Thug;  // Default

        if (typeName == "thug") {
            type = CharacterType::Thug;
        } else if (typeName == "cop") {
            type = CharacterType::Cop;
        } else if (typeName == "civilian") {
            type = CharacterType::Civilian;
        } else {
            if (m_logger) {
                m_logger->Warning("CharacterFactory: Unknown character type '" + typeName + 
                                "', defaulting to Thug");
            }
        }

        return CreateCharacter(type);
    }

    Engine::CharacterSpriteConfig CharacterFactory::GetConfigForType(CharacterType type) {
        switch (type) {
            case CharacterType::Thug:
                return CharacterConfigs::CreateThugConfig();
            
            case CharacterType::Cop:
                return CharacterConfigs::CreateCopConfig();
            
            case CharacterType::Civilian:
                return CharacterConfigs::CreateCivilianConfig();
            
            default:
                if (m_logger) {
                    m_logger->Error("CharacterFactory: Invalid character type");
                }
                return Engine::CharacterSpriteConfig();
        }
    }

    std::string CharacterFactory::GetTexturePathForType(CharacterType type) {
        switch (type) {
            case CharacterType::Thug:
                return Resources::Paths::Textures::THUG_SPRITESHEET;
            
            case CharacterType::Cop:
                // TODO: Add cop texture path when available
                return "Pics/Cop_Frames.png";
            
            case CharacterType::Civilian:
                // TODO: Add civilian texture path when available
                return "Pics/Civilian_Frames.png";
            
            default:
                return "";
        }
    }

    std::string CharacterFactory::GetResourceNameForType(CharacterType type) {
        return CharacterDataUtils::CharacterTypeToString(type) + "_spritesheet";
    }

} // namespace Entities
} // namespace LegalCrime
