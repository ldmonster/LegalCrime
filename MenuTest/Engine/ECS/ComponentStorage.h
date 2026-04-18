#pragma once

#include "EntityManager.h"
#include <vector>
#include <unordered_map>
#include <cassert>

namespace Engine {
namespace ECS {

    /// Dense-array component storage with sparse lookup.
    /// Provides cache-friendly iteration over all components of type T
    /// and O(1) lookup/add/remove by EntityId.
    template<typename T>
    class ComponentStorage {
    public:
        /// Add a component to an entity. Returns reference to the stored component.
        T& Add(EntityId id, T component = T{}) {
            assert(id != INVALID_ENTITY);
            auto it = m_sparse.find(id);
            if (it != m_sparse.end()) {
                // Overwrite existing
                m_dense[it->second].second = std::move(component);
                return m_dense[it->second].second;
            }
            size_t idx = m_dense.size();
            m_dense.push_back({id, std::move(component)});
            m_sparse[id] = idx;
            return m_dense[idx].second;
        }

        /// Remove a component from an entity. Swaps with last element for O(1).
        void Remove(EntityId id) {
            auto it = m_sparse.find(id);
            if (it == m_sparse.end()) return;

            size_t idx = it->second;
            size_t last = m_dense.size() - 1;

            if (idx != last) {
                // Swap with last element
                m_dense[idx] = std::move(m_dense[last]);
                m_sparse[m_dense[idx].first] = idx;
            }
            m_dense.pop_back();
            m_sparse.erase(it);
        }

        /// Get a pointer to an entity's component, or nullptr if not present.
        T* Get(EntityId id) {
            auto it = m_sparse.find(id);
            return (it != m_sparse.end()) ? &m_dense[it->second].second : nullptr;
        }

        const T* Get(EntityId id) const {
            auto it = m_sparse.find(id);
            return (it != m_sparse.end()) ? &m_dense[it->second].second : nullptr;
        }

        /// Check if an entity has this component.
        bool Has(EntityId id) const {
            return m_sparse.find(id) != m_sparse.end();
        }

        /// Number of components stored.
        size_t Size() const { return m_dense.size(); }

        /// Iterate all (EntityId, T&) pairs.
        template<typename Func>
        void ForEach(Func&& func) {
            for (auto& [id, component] : m_dense) {
                func(id, component);
            }
        }

        template<typename Func>
        void ForEach(Func&& func) const {
            for (const auto& [id, component] : m_dense) {
                func(id, component);
            }
        }

        /// Clear all stored components.
        void Clear() {
            m_dense.clear();
            m_sparse.clear();
        }

        /// Direct access to dense array for cache-friendly iteration.
        const std::vector<std::pair<EntityId, T>>& GetDenseArray() const { return m_dense; }

    private:
        std::vector<std::pair<EntityId, T>> m_dense;
        std::unordered_map<EntityId, size_t> m_sparse;
    };

} // namespace ECS
} // namespace Engine
