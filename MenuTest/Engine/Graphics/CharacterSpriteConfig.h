#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace Engine {

    /// <summary>
    /// Defines a single animation within a sprite sheet.
    /// </summary>
    struct AnimationConfig {
        std::string name;           // Animation name (e.g., "walk_left", "idle", "attack")
        uint16_t frameWidth;        // Width of each frame in pixels
        uint16_t frameHeight;       // Height of each frame in pixels
        uint16_t sheetColumns;      // Total columns in the sprite sheet
        uint16_t sheetRows;         // Total rows in the sprite sheet
        uint16_t startFrame;        // Starting frame index (0-based)
        uint16_t endFrame;          // Ending frame index (inclusive)
        float frameDuration;        // Duration of each frame in seconds
        bool loop;                  // Whether the animation should loop

        AnimationConfig()
            : name("")
            , frameWidth(0)
            , frameHeight(0)
            , sheetColumns(0)
            , sheetRows(0)
            , startFrame(0)
            , endFrame(0)
            , frameDuration(0.1f)
            , loop(true) {
        }

        AnimationConfig(
            const std::string& animName,
            uint16_t width,
            uint16_t height,
            uint16_t cols,
            uint16_t rows,
            uint16_t start,
            uint16_t end,
            float duration,
            bool shouldLoop)
            : name(animName)
            , frameWidth(width)
            , frameHeight(height)
            , sheetColumns(cols)
            , sheetRows(rows)
            , startFrame(start)
            , endFrame(end)
            , frameDuration(duration)
            , loop(shouldLoop) {
        }
    };

    /// <summary>
    /// Configuration for a complete character sprite with all its animations.
    /// </summary>
    struct CharacterSpriteConfig {
        std::string characterType;          // Character type identifier (e.g., "thug", "cop", "civilian")
        std::string spriteSheetPath;        // Path to the sprite sheet texture
        float defaultScale;                 // Default scale factor for the sprite
        std::vector<AnimationConfig> animations;  // All animations for this character
        std::string defaultAnimation;       // Name of the default/idle animation

        CharacterSpriteConfig()
            : characterType("")
            , spriteSheetPath("")
            , defaultScale(1.0f)
            , defaultAnimation("idle") {
        }
    };

} // namespace Engine
