#pragma once

#include "../Engine/Graphics/CharacterSpriteConfig.h"

namespace LegalCrime {

    /// <summary>
    /// Predefined character sprite configurations for the game.
    /// Add new character types here to make them available throughout the game.
    /// </summary>
    class CharacterConfigs {
    private:
        // Thug sprite sheet constants
        static constexpr uint16_t THUG_SPRITE_WIDTH = 50;
        static constexpr uint16_t THUG_SPRITE_HEIGHT = 50;
        static constexpr uint16_t THUG_SPRITE_COLUMNS = 6;
        static constexpr uint16_t THUG_SPRITE_ROWS = 6;

    public:
        /// <summary>
        /// Creates the configuration for the Thug character sprite.
        /// Sprite sheet: Pics/Thug_Frames.png (6 columns x 4 rows, 50x50 per frame)
        /// </summary>
        static Engine::CharacterSpriteConfig CreateThugConfig() {
            Engine::CharacterSpriteConfig config;
            config.characterType = "thug";
            config.spriteSheetPath = "Pics/Thug_Frames.png";
            config.defaultScale = 2.0f;
            config.defaultAnimation = "idle";

            // Walk down animation (frames 0-6)
            config.animations.push_back(Engine::AnimationConfig(
                "walk_down",
                THUG_SPRITE_WIDTH,
                THUG_SPRITE_HEIGHT,
                THUG_SPRITE_COLUMNS,
                THUG_SPRITE_ROWS,
                0,           // start frame
                6,           // end frame
                0.1f,
                true
            ));

            // Walk right animation (frames 7-12)
            config.animations.push_back(Engine::AnimationConfig(
                "walk_right",
                THUG_SPRITE_WIDTH,
                THUG_SPRITE_HEIGHT,
                THUG_SPRITE_COLUMNS,
                THUG_SPRITE_ROWS,
                7,           // start frame
                12,          // end frame
                0.1f,
                true
            ));

            // Walk up animation (frames 13-18)
            config.animations.push_back(Engine::AnimationConfig(
                "walk_up",
                THUG_SPRITE_WIDTH,
                THUG_SPRITE_HEIGHT,
                THUG_SPRITE_COLUMNS,
                THUG_SPRITE_ROWS,
                13,          // start frame
                18,          // end frame
                0.1f,
                true
            ));

            // Walk left animation (frames 19-24)
            config.animations.push_back(Engine::AnimationConfig(
                "walk_left",
                THUG_SPRITE_WIDTH,
                THUG_SPRITE_HEIGHT,
                THUG_SPRITE_COLUMNS,
                THUG_SPRITE_ROWS,
                19,          // start frame
                24,          // end frame
                0.1f,
                true
            ));

            // Idle animation (frames 0-2)
            config.animations.push_back(Engine::AnimationConfig(
                "idle",
                THUG_SPRITE_WIDTH,
                THUG_SPRITE_HEIGHT,
                THUG_SPRITE_COLUMNS,
                THUG_SPRITE_ROWS,
                25,           // start frame
                36,           // end frame
                0.2f,        // frame duration
                true         // loop
            ));

            return config;
        }

        /// <summary>
        /// Example: Create configuration for a Cop character.
        /// Modify this template for your actual cop sprite sheet.
        /// </summary>
        static Engine::CharacterSpriteConfig CreateCopConfig() {
            Engine::CharacterSpriteConfig config;
            config.characterType = "cop";
            config.spriteSheetPath = "Pics/Cop_Frames.png";  // Update with actual path
            config.defaultScale = 2.0f;
            config.defaultAnimation = "idle";

            // TODO: Add animations specific to cop sprite sheet
            // Example template:
            config.animations.push_back(Engine::AnimationConfig(
                "idle",
                64, 64,      // Adjust frame size
                8, 4,        // Adjust columns/rows
                0, 3,        // Adjust frame range
                0.15f,
                true
            ));

            return config;
        }

        /// <summary>
        /// Example: Create configuration for a Civilian character.
        /// Modify this template for your actual civilian sprite sheet.
        /// </summary>
        static Engine::CharacterSpriteConfig CreateCivilianConfig() {
            Engine::CharacterSpriteConfig config;
            config.characterType = "civilian";
            config.spriteSheetPath = "Pics/Civilian_Frames.png";  // Update with actual path
            config.defaultScale = 2.0f;
            config.defaultAnimation = "idle";

            // TODO: Add animations specific to civilian sprite sheet
            // Copy the pattern from CreateThugConfig() and adjust for your sprite sheet

            return config;
        }

        // Add more character configuration methods here as you add new character types
    };

} // namespace LegalCrime
