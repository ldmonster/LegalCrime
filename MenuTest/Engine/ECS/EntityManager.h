#pragma once

#include "../Core/Types.h"
#include <vector>
#include <cstdint>

namespace Engine {
namespace ECS {

    using EntityId = uint32_t;
    static constexpr EntityId INVALID_ENTITY = 0;

    /// Versioned handle that detects stale references to recycled entity IDs.
    struct EntityHandle {
        EntityId id;
        uint32_t version;

        EntityHandle() : id(INVALID_ENTITY), version(0) {}
        EntityHandle(EntityId i, uint32_t v) : id(i), version(v) {}

        bool operator==(const EntityHandle& o) const { return id == o.id && version == o.version; }
        bool operator!=(const EntityHandle& o) const { return !(*this == o); }
        explicit operator bool() const { return id != INVALID_ENTITY; }

        struct Hash {
            std::size_t operator()(const EntityHandle& h) const {
                return std::hash<uint64_t>()(
                    (static_cast<uint64_t>(h.id) << 32) | h.version);
            }
        };
    };

    static constexpr EntityHandle INVALID_HANDLE = EntityHandle();

    /// Lightweight entity manager. Tracks alive entities and recycles IDs.
    /// Uses a free-list for O(1) create/destroy.
    /// Versioned handles detect use-after-destroy.
    class EntityManager {
    public:
        EntityManager() : m_nextId(1) {}

        EntityHandle CreateEntity() {
            EntityId id;
            if (!m_freeList.empty()) {
                id = m_freeList.back();
                m_freeList.pop_back();
            } else {
                id = m_nextId++;
            }
            if (id >= m_alive.size()) {
                m_alive.resize(static_cast<size_t>(id) + 1, false);
                m_versions.resize(static_cast<size_t>(id) + 1, 0);
            }
            m_alive[id] = true;
            ++m_count;
            return EntityHandle(id, m_versions[id]);
        }

        void DestroyEntity(EntityHandle handle) {
            EntityId id = handle.id;
            if (id == INVALID_ENTITY || id >= m_alive.size() || !m_alive[id]) return;
            if (handle.version != m_versions[id]) return; // stale handle
            m_alive[id] = false;
            m_versions[id]++; // increment version so old handles become stale
            m_freeList.push_back(id);
            --m_count;
        }

        /// Legacy overload for raw IDs (use handle version when possible).
        void DestroyEntity(EntityId id) {
            if (id == INVALID_ENTITY || id >= m_alive.size() || !m_alive[id]) return;
            m_alive[id] = false;
            m_versions[id]++;
            m_freeList.push_back(id);
            --m_count;
        }

        bool IsAlive(EntityHandle handle) const {
            EntityId id = handle.id;
            return id != INVALID_ENTITY && id < m_alive.size() 
                && m_alive[id] && handle.version == m_versions[id];
        }

        bool IsAlive(EntityId id) const {
            return id != INVALID_ENTITY && id < m_alive.size() && m_alive[id];
        }

        /// Get the current version for an entity slot.
        uint32_t GetVersion(EntityId id) const {
            return (id < m_versions.size()) ? m_versions[id] : 0;
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
        std::vector<uint32_t> m_versions;
        std::vector<EntityId> m_freeList;
        EntityId m_nextId;
        size_t m_count = 0;
    };

} // namespace ECS
} // namespace Engine
