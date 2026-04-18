#pragma once

#include "../../Engine/Graphics/CharacterSpriteConfig.h"
#include <string>
#include <vector>

namespace LegalCrime {
namespace Entities {

    /// <summary>
    /// Defines character types available in the game.
    /// </summary>
    enum class CharacterType {
        Thug,
        Cop,
        Civilian,
        Count  // Number of character types
    };

    /// Direction is now defined in Engine::Direction (Engine/Core/Types.h).
    /// This alias preserves backward compatibility within the Entities namespace.
    using Direction = Engine::Direction;

    /// <summary>
    /// Character definition including sprite configuration and gameplay data.
    /// </summary>
    struct CharacterData {
        CharacterType type;
        std::string name;
        Engine::CharacterSpriteConfig spriteConfig;
        
        CharacterData()
            : type(CharacterType::Thug)
            , name("Unknown")
            , spriteConfig() {
        }
    };

    /// <summary>
    /// Utility functions for character data.
    /// </summary>
    namespace CharacterDataUtils {
        
        /// <summary>
        /// Convert CharacterType enum to string.
        /// </summary>
        inline std::string CharacterTypeToString(CharacterType type) {
            switch (type) {
                case CharacterType::Thug: return "thug";
                case CharacterType::Cop: return "cop";
                case CharacterType::Civilian: return "civilian";
                default: return "unknown";
            }
        }

        /// Convert Direction enum to string.
        inline std::string DirectionToString(Direction dir) {
            return Engine::DirectionUtil::ToString(dir);
        }

        /// Get animation name for direction.
        inline std::string GetWalkAnimationForDirection(Direction dir) {
            return Engine::DirectionUtil::ToAnimationName(dir, "walk");
        }
    }

} // namespace Entities
} // namespace LegalCrime
