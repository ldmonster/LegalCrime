#include "GameResources.h"

namespace LegalCrime {
namespace Resources {

    const char* ResourceIDs::GetTextureName(TextureID id) {
        switch (id) {
            case TextureID::ThugSpritesheet: return "ThugSpritesheet";
            case TextureID::CopSpritesheet: return "CopSpritesheet";
            case TextureID::CivilianSpritesheet: return "CivilianSpritesheet";
            case TextureID::ButtonNormal: return "ButtonNormal";
            case TextureID::ButtonHover: return "ButtonHover";
            case TextureID::ButtonPressed: return "ButtonPressed";
            case TextureID::ButtonDisabled: return "ButtonDisabled";
            case TextureID::CaponeIcon: return "CaponeIcon";
            case TextureID::TileGrass: return "TileGrass";
            case TextureID::TileStone: return "TileStone";
            case TextureID::TileWater: return "TileWater";
            default: return "Unknown";
        }
    }

    const char* ResourceIDs::GetSoundName(SoundID id) {
        switch (id) {
            case SoundID::ButtonClick: return "ButtonClick";
            case SoundID::ButtonHover: return "ButtonHover";
            case SoundID::Footstep: return "Footstep";
            case SoundID::Jump: return "Jump";
            case SoundID::Land: return "Land";
            case SoundID::Punch: return "Punch";
            case SoundID::Gunshot: return "Gunshot";
            case SoundID::Reload: return "Reload";
            default: return "Unknown";
        }
    }

    const char* ResourceIDs::GetMusicName(MusicID id) {
        switch (id) {
            case MusicID::MainMenu: return "MainMenu";
            case MusicID::Gameplay: return "Gameplay";
            case MusicID::Combat: return "Combat";
            case MusicID::Victory: return "Victory";
            case MusicID::GameOver: return "GameOver";
            default: return "Unknown";
        }
    }

} // namespace Resources
} // namespace LegalCrime
