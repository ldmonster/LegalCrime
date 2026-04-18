#pragma once

#include "../Commands/Command.h"
#include "../../../Engine/Core/Types.h"
#include <unordered_map>
#include <cstdint>

namespace Engine {
    class ILogger;
}

namespace LegalCrime {
namespace Entities {
    class Character;
}
namespace World {
    class World;
    class MovementSystem;

    /// CommandSystem manages per-unit command queues and executes the
    /// front command each frame by delegating to the appropriate subsystem.
    class CommandSystem {
    public:
        CommandSystem(Engine::ILogger* logger = nullptr);
        ~CommandSystem();

        /// Process all queued commands for entities that are idle.
        void Update(World* world, MovementSystem* movement, float deltaTime);

        /// Issue a command to a unit (replaces current queue).
        void IssueCommand(uint32_t entityId, const Command& cmd);

        /// Queue a command (shift-click style, appends to back).
        void QueueCommand(uint32_t entityId, const Command& cmd);

        /// Cancel all commands for a unit.
        void CancelCommands(uint32_t entityId);

        /// Get the command queue for a unit (or nullptr).
        const CommandQueue* GetQueue(uint32_t entityId) const;

        /// Check if a unit has pending commands.
        bool HasCommands(uint32_t entityId) const;

    private:
        void ProcessMoveCommand(Entities::Character* character, const Command& cmd,
                                MovementSystem* movement);

        Engine::ILogger* m_logger;
        std::unordered_map<uint32_t, CommandQueue> m_queues;
    };

} // namespace World
} // namespace LegalCrime
