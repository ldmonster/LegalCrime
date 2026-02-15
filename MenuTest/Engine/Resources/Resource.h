#pragma once

#include <string>
#include <memory>

namespace Engine {
namespace Resources {

    /// <summary>
    /// Base class for all managed resources.
    /// Provides common functionality like reference counting and resource lifecycle.
    /// </summary>
    class Resource {
    public:
        virtual ~Resource() = default;

        // Resource identification
        const std::string& GetName() const { return m_name; }
        const std::string& GetPath() const { return m_path; }

        // Resource state
        bool IsLoaded() const { return m_isLoaded; }
        size_t GetSize() const { return m_sizeInBytes; }

        // Reference counting (for manual tracking if needed)
        void AddRef() { ++m_refCount; }
        void Release() { 
            if (--m_refCount == 0) {
                Unload();
            }
        }
        int GetRefCount() const { return m_refCount; }

    protected:
        Resource(const std::string& name, const std::string& path)
            : m_name(name)
            , m_path(path)
            , m_isLoaded(false)
            , m_sizeInBytes(0)
            , m_refCount(0) {
        }

        // Override in derived classes
        virtual void Unload() { m_isLoaded = false; }

        std::string m_name;
        std::string m_path;
        bool m_isLoaded;
        size_t m_sizeInBytes;
        int m_refCount;
    };

} // namespace Resources
} // namespace Engine
