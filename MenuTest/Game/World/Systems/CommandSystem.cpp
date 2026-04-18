#include "CommandSystem.h"
#include "MovementSystem.h"
#include "../World.h"
#include "../../Entities/Character.h"
#include "../../../Engine/Core/Logger/ILogger.h"

namespace LegalCrime {
namespace World {

    CommandSystem::CommandSystem(Engine::ILogger* logger)
        : m_logger(logger) {
    }

    CommandSystem::~CommandSystem() = default;

    void CommandSystem::Update(World* world, MovementSystem* movement, float deltaTime) {
        if (!world || !movement) return;

        std::vector<uint32_t> completedQueues;

        for (auto& [entityId, queue] : m_queues) {
            if (queue.IsEmpty()) {
                completedQueues.push_back(entityId);
                continue;
            }

            auto* character = world->GetCharacterById(entityId);
            if (!character) {
                completedQueues.push_back(entityId);
                continue;
            }

            const Command* cmd = queue.GetCurrentCommand();
            if (!cmd) continue;

            switch (cmd->type) {
                case CommandType::Move: {
                    // If not already moving, start movement
                    if (!movement->IsCharacterMoving(character)) {
                        ProcessMoveCommand(character, *cmd, movement);
                        queue.PopCommand();
                    }
                    break;
                }
                case CommandType::Stop: {
                    movement->StopCharacterMovement(character);
                    queue.PopCommand();
                    break;
                }
                case CommandType::Attack:
                case CommandType::Patrol:
                case CommandType::Hold:
                case CommandType::Gather:
                case CommandType::Build:
                    // Not yet implemented — pop to avoid infinite loop
                    queue.PopCommand();
                    break;
            }
        }

        // Clean up empty queues
        for (uint32_t id : completedQueues) {
            m_queues.erase(id);
        }
    }

    void CommandSystem::IssueCommand(uint32_t entityId, const Command& cmd) {
        m_queues[entityId].SetCommand(cmd);
    }

    void CommandSystem::QueueCommand(uint32_t entityId, const Command& cmd) {
        m_queues[entityId].QueueCommand(cmd);
    }

    void CommandSystem::CancelCommands(uint32_t entityId) {
        auto it = m_queues.find(entityId);
        if (it != m_queues.end()) {
            it->second.Clear();
        }
    }

    const CommandQueue* CommandSystem::GetQueue(uint32_t entityId) const {
        auto it = m_queues.find(entityId);
        return (it != m_queues.end()) ? &it->second : nullptr;
    }

    bool CommandSystem::HasCommands(uint32_t entityId) const {
        auto it = m_queues.find(entityId);
        return it != m_queues.end() && !it->second.IsEmpty();
    }

    void CommandSystem::ProcessMoveCommand(Entities::Character* character, const Command& cmd,
                                            MovementSystem* movement) {
        if (!character || !movement) return;

        movement->MoveCharacterToTile(character, cmd.targetPosition);

        if (m_logger) {
            m_logger->Debug("CommandSystem: Move command issued to (" +
                          std::to_string(cmd.targetPosition.row) + "," +
                          std::to_string(cmd.targetPosition.col) + ")");
        }
    }

} // namespace World
} // namespace LegalCrime
