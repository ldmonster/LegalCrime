#pragma once

#include "../../../Engine/Core/Types.h"
#include <cstdint>
#include <deque>
#include <variant>
#include <vector>

namespace LegalCrime {
namespace World {

    struct MoveCommand {
        Engine::TilePosition target;
        float moveDuration;

        MoveCommand(const Engine::TilePosition& pos = Engine::TilePosition(), float duration = 0.3f)
            : target(pos)
            , moveDuration(duration) {}
    };

    struct AttackCommand {
        uint32_t targetEntityId;
        float attackRange;

        AttackCommand(uint32_t targetId = 0, float range = 1.0f)
            : targetEntityId(targetId)
            , attackRange(range) {}
    };

    struct PatrolCommand {
        std::vector<Engine::TilePosition> waypoints;
    };

    struct StopCommand {};
    struct HoldCommand {};
    struct GatherCommand {};
    struct BuildCommand {};

    using GameCommand = std::variant<
        MoveCommand,
        AttackCommand,
        PatrolCommand,
        StopCommand,
        HoldCommand,
        GatherCommand,
        BuildCommand
    >;

    /// FIFO command queue for a single unit.
    /// Supports shift-queue appending and immediate replacement.
    class CommandQueue {
    public:
        /// Replace all commands with a single new command.
        void SetCommand(const GameCommand& cmd) {
            m_commands.clear();
            m_commands.push_back(cmd);
        }

        /// Append a command to the back of the queue (shift-click behavior).
        void QueueCommand(const GameCommand& cmd) {
            m_commands.push_back(cmd);
        }

        /// Get the current (front) command, or nullptr if empty.
        const GameCommand* GetCurrentCommand() const {
            return m_commands.empty() ? nullptr : &m_commands.front();
        }

        /// Pop the current command (when completed or cancelled).
        void PopCommand() {
            if (!m_commands.empty()) {
                m_commands.pop_front();
            }
        }

        /// Clear all queued commands.
        void Clear() { m_commands.clear(); }

        /// Check if the queue is empty.
        bool IsEmpty() const { return m_commands.empty(); }

        /// Number of queued commands.
        size_t GetCount() const { return m_commands.size(); }

    private:
        std::deque<GameCommand> m_commands;
    };

} // namespace World
} // namespace LegalCrime
