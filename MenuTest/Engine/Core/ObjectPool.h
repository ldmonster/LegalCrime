#pragma once

#include <vector>
#include <cstddef>
#include <cassert>

namespace Engine {

    /// Fixed-capacity pre-allocated object pool.
    /// Objects are stored in a contiguous flat array for cache-friendly access.
    /// Call Reset() between queries instead of freeing individual objects.
    /// @tparam T Must be default-constructible.
    template<typename T>
    class ObjectPool {
    public:
        explicit ObjectPool(size_t capacity)
            : m_pool(capacity)
            , m_count(0) {
        }

        /// Acquire the next object from the pool. Returns nullptr if full.
        T* Acquire() {
            if (m_count >= m_pool.size()) return nullptr;
            T* obj = &m_pool[m_count++];
            *obj = T{}; // reset to default state
            return obj;
        }

        /// Reset the pool — all acquired objects become invalid.
        void Reset() { m_count = 0; }

        size_t Count() const { return m_count; }
        size_t Capacity() const { return m_pool.size(); }

        /// Grow capacity if needed. Invalidates all previously acquired pointers.
        void Reserve(size_t newCapacity) {
            if (newCapacity > m_pool.size()) {
                m_pool.resize(newCapacity);
            }
        }

    private:
        std::vector<T> m_pool;
        size_t m_count;
    };

} // namespace Engine
