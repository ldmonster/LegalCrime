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

            const GameCommand* cmd = queue.GetCurrentCommand();
            if (!cmd) continue;

            bool consumed = false;
            std::visit([&](const auto& concreteCmd) {
                using T = std::decay_t<decltype(concreteCmd)>;

                if constexpr (std::is_same_v<T, MoveCommand>) {
                    if (!movement->IsCharacterMoving(character)) {
                        ProcessMoveCommand(character, concreteCmd, movement);
                        consumed = true;
                    }
                } else if constexpr (std::is_same_v<T, StopCommand>) {
                    movement->StopCharacterMovement(character);
                    consumed = true;
                } else {
                    // Not implemented yet.
                    consumed = true;
                }
            }, *cmd);

            if (consumed) {
                queue.PopCommand();
            }
        }

        // Clean up empty queues
        for (uint32_t id : completedQueues) {
            m_queues.erase(id);
        }
    }

    void CommandSystem::IssueCommand(uint32_t entityId, const GameCommand& cmd) {
        m_queues[entityId].SetCommand(cmd);
    }

    void CommandSystem::QueueCommand(uint32_t entityId, const GameCommand& cmd) {
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

    void CommandSystem::ProcessMoveCommand(Entities::Character* character, const MoveCommand& cmd,
                                            MovementSystem* movement) {
        if (!character || !movement) return;

        movement->MoveCharacterToTile(character, cmd.target, cmd.moveDuration);

        if (m_logger) {
            m_logger->Debug("CommandSystem: Move command issued to (" +
                          std::to_string(cmd.target.row) + "," +
                          std::to_string(cmd.target.col) + ")");
        }
    }

} // namespace World
} // namespace LegalCrime
