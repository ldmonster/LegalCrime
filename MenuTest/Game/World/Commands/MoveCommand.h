#pragma once

#include "Command.h"
#include "../../GameConstants.h"

namespace LegalCrime {
namespace World {

    /// Move command — orders a unit to move to a target tile.
    struct MoveCommand : public Command {
        float moveDuration;  // Time per tile transition

        MoveCommand()
            : Command(CommandType::Move, Engine::TilePosition())
            , moveDuration(Constants::Movement::DEFAULT_MOVE_DURATION) {}

        MoveCommand(const Engine::TilePosition& target, float duration = Constants::Movement::DEFAULT_MOVE_DURATION)
            : Command(CommandType::Move, target)
            , moveDuration(duration) {}
    };

} // namespace World
} // namespace LegalCrime
