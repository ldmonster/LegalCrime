#pragma once

#include "../../../Engine/Core/Types.h"
#include <cstdint>
#include <deque>

namespace LegalCrime {
namespace World {

    /// Command types for the RTS command queue.
    enum class CommandType : uint8_t {
        Move,
        Attack,
        Patrol,
        Stop,
        Hold,
        Gather,
        Build
    };

    /// Base command data. All commands derive from this.
    struct Command {
        CommandType type;
        uint32_t targetEntityId;        // Target entity (0 = none)
        Engine::TilePosition targetPosition; // Target tile
        float priority;                 // Higher = executed first (default 0)

        Command()
            : type(CommandType::Stop)
            , targetEntityId(0)
            , targetPosition()
            , priority(0.0f) {}

        Command(CommandType t, const Engine::TilePosition& pos, uint32_t targetId = 0, float prio = 0.0f)
            : type(t)
            , targetEntityId(targetId)
            , targetPosition(pos)
            , priority(prio) {}

        virtual ~Command() = default;
    };

    /// FIFO command queue for a single unit.
    /// Supports shift-queue appending and immediate replacement.
    class CommandQueue {
    public:
        /// Replace all commands with a single new command.
        void SetCommand(const Command& cmd) {
            m_commands.clear();
            m_commands.push_back(cmd);
        }

        /// Append a command to the back of the queue (shift-click behavior).
        void QueueCommand(const Command& cmd) {
            m_commands.push_back(cmd);
        }

        /// Get the current (front) command, or nullptr if empty.
        const Command* GetCurrentCommand() const {
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
        std::deque<Command> m_commands;
    };

} // namespace World
} // namespace LegalCrime
