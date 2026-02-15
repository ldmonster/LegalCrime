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

    /// <summary>
    /// Direction the character is facing.
    /// </summary>
    enum class Direction {
        Down = 0,
        Right = 1,
        Up = 2,
        Left = 3
    };

    /// <summary>
    /// Character definition including sprite configuration and gameplay data.
    /// </summary>
    struct CharacterData {
        CharacterType type;
        std::string name;
        Engine::CharacterSpriteConfig spriteConfig;
        
        // Gameplay attributes (for future use)
        float moveSpeed;
        int health;
        
        CharacterData()
            : type(CharacterType::Thug)
            , name("Unknown")
            , spriteConfig()
            , moveSpeed(100.0f)
            , health(100) {
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

        /// <summary>
        /// Convert Direction enum to string.
        /// </summary>
        inline std::string DirectionToString(Direction dir) {
            switch (dir) {
                case Direction::Down: return "down";
                case Direction::Right: return "right";
                case Direction::Up: return "up";
                case Direction::Left: return "left";
                default: return "down";
            }
        }

        /// <summary>
        /// Get animation name for direction.
        /// </summary>
        inline std::string GetWalkAnimationForDirection(Direction dir) {
            return "walk_" + DirectionToString(dir);
        }
    }

} // namespace Entities
} // namespace LegalCrime
