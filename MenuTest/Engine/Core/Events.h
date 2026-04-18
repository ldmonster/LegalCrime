#pragma once

#include "Types.h"
#include <cstdint>
#include <string>

namespace Engine {

    /// Fired when a character finishes moving to a new tile.
    struct CharacterMovedEvent {
        uint32_t characterId;
        TilePosition from;
        TilePosition to;
    };

    /// Fired when a character is selected or deselected.
    struct CharacterSelectedEvent {
        uint32_t characterId;  // 0 if deselected
    };

    /// Fired when a scene transition is requested.
    struct SceneTransitionEvent {
        std::string targetScene;
    };

} // namespace Engine
