#pragma once

#include "../Core/Types.h"
#include <vector>
#include <cstdint>

namespace Engine {
namespace ECS {

    using EntityId = uint32_t;
    static constexpr EntityId INVALID_ENTITY = 0;

    /// Lightweight entity manager. Tracks alive entities and recycles IDs.
    /// Uses a free-list for O(1) create/destroy.
    class EntityManager {
    public:
        EntityManager() : m_nextId(1) {}

        EntityId CreateEntity() {
            EntityId id;
            if (!m_freeList.empty()) {
                id = m_freeList.back();
                m_freeList.pop_back();
            } else {
                id = m_nextId++;
            }
            if (id >= m_alive.size()) {
                m_alive.resize(static_cast<size_t>(id) + 1, false);
            }
            m_alive[id] = true;
            ++m_count;
            return id;
        }

        void DestroyEntity(EntityId id) {
            if (id == INVALID_ENTITY || id >= m_alive.size() || !m_alive[id]) return;
            m_alive[id] = false;
            m_freeList.push_back(id);
            --m_count;
        }

        bool IsAlive(EntityId id) const {
            return id != INVALID_ENTITY && id < m_alive.size() && m_alive[id];
        }

        size_t GetEntityCount() const { return m_count; }

        /// Iterate all alive entity IDs. Callback: void(EntityId).
        template<typename Func>
        void ForEach(Func&& func) const {
            for (EntityId id = 1; id < static_cast<EntityId>(m_alive.size()); ++id) {
                if (m_alive[id]) func(id);
            }
        }

    private:
        std::vector<bool> m_alive;
        std::vector<EntityId> m_freeList;
        EntityId m_nextId;
        size_t m_count = 0;
    };

} // namespace ECS
} // namespace Engine
