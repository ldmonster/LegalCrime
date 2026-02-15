#pragma once

namespace LegalCrime {
namespace Resources {

    /// <summary>
    /// Centralized resource path definitions.
    /// All asset paths should be defined here for easy maintenance.
    /// </summary>
    namespace Paths {

        // Textures
        namespace Textures {
            constexpr const char* THUG_SPRITESHEET = "Pics/Thug_Frames.png";
            constexpr const char* COP_SPRITESHEET = "Pics/Cop_Frames.png";
            constexpr const char* CIVILIAN_SPRITESHEET = "Pics/Civilian_Frames.png";
            constexpr const char* CAPONE_ICON = "Pics/capone.ico";

            // UI textures (example placeholders)
            constexpr const char* BUTTON_NORMAL = "Pics/UI/button_normal.png";
            constexpr const char* BUTTON_HOVER = "Pics/UI/button_hover.png";
            constexpr const char* BUTTON_PRESSED = "Pics/UI/button_pressed.png";
        }

        // Audio - Music
        namespace Music {
            constexpr const char* MAIN_MENU = "Music/main_menu.mp3";
            constexpr const char* GAMEPLAY = "Music/gameplay.mp3";
            constexpr const char* COMBAT = "Music/combat.mp3";
        }

        // Audio - Sound Effects
        namespace Sounds {
            constexpr const char* BUTTON_CLICK = "Sound/button_click.wav";
            constexpr const char* FOOTSTEP = "Sound/footstep.wav";
            constexpr const char* GUNSHOT = "Sound/gunshot.wav";
            constexpr const char* PUNCH = "Sound/punch.wav";
        }

        // Fonts
        namespace Fonts {
            constexpr const char* MAIN_FONT = "Fonts/arial.ttf";
            constexpr const char* TITLE_FONT = "Fonts/title.ttf";
        }

        // Data files
        namespace Data {
            constexpr const char* CHARACTERS = "Data/characters.json";
            constexpr const char* LEVELS = "Data/levels.json";
            constexpr const char* ITEMS = "Data/items.json";
        }

    } // namespace Paths

} // namespace Resources
} // namespace LegalCrime
