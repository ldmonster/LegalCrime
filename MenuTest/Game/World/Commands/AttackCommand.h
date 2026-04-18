#pragma once

#include "Command.h"

namespace LegalCrime {
namespace World {

    /// Attack command — orders a unit to attack a target entity.
    struct AttackCommand : public Command {
        float attackRange;

        AttackCommand()
            : Command(CommandType::Attack, Engine::TilePosition())
            , attackRange(1.0f) {}

        AttackCommand(uint32_t targetId, float range = 1.0f)
            : Command(CommandType::Attack, Engine::TilePosition(), targetId)
            , attackRange(range) {}

        AttackCommand(uint32_t targetId, const Engine::TilePosition& pos, float range = 1.0f)
            : Command(CommandType::Attack, pos, targetId)
            , attackRange(range) {}
    };

} // namespace World
} // namespace LegalCrime
