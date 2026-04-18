#pragma once

#include "../Engine/Graphics/CharacterSpriteConfig.h"
#include "../Engine/Core/Types.h"

namespace LegalCrime {

    /// Table-driven character sprite configurations.
    /// Add new character types by adding a new entry to the tables below.
    class CharacterConfigs {
    public:
        /// Walk animation frame ranges per direction (Down, Right, Up, Left).
        struct WalkFrameTable {
            uint16_t startFrames[4];
            uint16_t endFrames[4];
        };

        /// Full character sprite definition.
        struct CharacterDef {
            const char* type;
            const char* spritePath;
            float defaultScale;
            uint16_t frameWidth;
            uint16_t frameHeight;
            uint16_t sheetColumns;
            uint16_t sheetRows;
            WalkFrameTable walk;
            uint16_t idleStart;
            uint16_t idleEnd;
            float walkFrameDuration;
            float idleFrameDuration;
        };

        static Engine::CharacterSpriteConfig BuildConfig(const CharacterDef& def) {
            Engine::CharacterSpriteConfig config;
            config.characterType = def.type;
            config.spriteSheetPath = def.spritePath;
            config.defaultScale = def.defaultScale;
            config.defaultAnimation = "idle";

            // Walk animations for each direction
            constexpr Engine::Direction dirs[] = {
                Engine::Direction::Down, Engine::Direction::Right,
                Engine::Direction::Up,   Engine::Direction::Left
            };

            for (int i = 0; i < 4; ++i) {
                config.animations.push_back(Engine::AnimationConfig(
                    Engine::DirectionUtil::ToAnimationName(dirs[i]),
                    def.frameWidth, def.frameHeight,
                    def.sheetColumns, def.sheetRows,
                    def.walk.startFrames[i], def.walk.endFrames[i],
                    def.walkFrameDuration, true
                ));
            }

            // Idle animation
            config.animations.push_back(Engine::AnimationConfig(
                "idle",
                def.frameWidth, def.frameHeight,
                def.sheetColumns, def.sheetRows,
                def.idleStart, def.idleEnd,
                def.idleFrameDuration, true
            ));

            return config;
        }

        // ---- Character definitions (data table) ----

        static constexpr CharacterDef THUG_DEF = {
            "thug", "Pics/Thug_Frames.png", 2.0f,
            50, 50, 6, 6,
            { {0, 7, 13, 19}, {6, 12, 18, 24} },  // walk start/end per direction
            25, 36,    // idle frames
            0.1f, 0.2f // walk/idle frame duration
        };

        static constexpr CharacterDef COP_DEF = {
            "cop", "Pics/Cop_Frames.png", 2.0f,
            64, 64, 8, 4,
            { {0, 0, 0, 0}, {3, 3, 3, 3} },  // placeholder
            0, 3,
            0.1f, 0.15f
        };

        static constexpr CharacterDef CIVILIAN_DEF = {
            "civilian", "Pics/Civilian_Frames.png", 2.0f,
            64, 64, 8, 4,
            { {0, 0, 0, 0}, {3, 3, 3, 3} },  // placeholder
            0, 3,
            0.1f, 0.15f
        };

        // ---- Factory methods (backward compatible) ----

        static Engine::CharacterSpriteConfig CreateThugConfig() {
            return BuildConfig(THUG_DEF);
        }

        static Engine::CharacterSpriteConfig CreateCopConfig() {
            return BuildConfig(COP_DEF);
        }

        static Engine::CharacterSpriteConfig CreateCivilianConfig() {
            return BuildConfig(CIVILIAN_DEF);
        }
    };

} // namespace LegalCrime
