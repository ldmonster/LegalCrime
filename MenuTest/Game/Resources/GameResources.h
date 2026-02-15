#pragma once

namespace LegalCrime {
namespace Resources {

    /// <summary>
    /// Resource identifier enums for type-safe resource access.
    /// Use these IDs instead of string names for better refactoring support.
    /// </summary>

    enum class TextureID {
        // Characters
        ThugSpritesheet,
        CopSpritesheet,
        CivilianSpritesheet,

        // UI
        ButtonNormal,
        ButtonHover,
        ButtonPressed,
        ButtonDisabled,
        
        // Icons
        CaponeIcon,
        
        // World
        TileGrass,
        TileStone,
        TileWater,
        
        // Count (always last)
        Count
    };

    enum class SoundID {
        // UI
        ButtonClick,
        ButtonHover,
        
        // Character
        Footstep,
        Jump,
        Land,
        
        // Combat
        Punch,
        Gunshot,
        Reload,
        
        // Count (always last)
        Count
    };

    enum class MusicID {
        MainMenu,
        Gameplay,
        Combat,
        Victory,
        GameOver,
        
        // Count (always last)
        Count
    };

    /// <summary>
    /// Helper to convert enum IDs to string names.
    /// </summary>
    class ResourceIDs {
    public:
        static const char* GetTextureName(TextureID id);
        static const char* GetSoundName(SoundID id);
        static const char* GetMusicName(MusicID id);
    };

} // namespace Resources
} // namespace LegalCrime
