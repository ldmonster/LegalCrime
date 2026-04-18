#pragma once

#include "Resource.h"
#include "../Graphics/Texture.h"
#include "../Core/Logger/ILogger.h"
#include "../Renderer/IRenderer.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <list>

namespace Engine {
namespace Resources {

    /// <summary>
    /// Texture resource wrapper.
    /// Wraps Engine::Texture for resource management.
    /// </summary>
    class TextureResource : public Resource {
    public:
        TextureResource(const std::string& name, const std::string& path, std::shared_ptr<Texture> texture)
            : Resource(name, path)
            , m_texture(texture) {
            if (m_texture) {
                m_isLoaded = true;
                m_sizeInBytes = m_texture->GetWidth() * m_texture->GetHeight() * 4; // Assume RGBA
            }
        }

        std::shared_ptr<Texture> GetTexture() const { return m_texture; }

    private:
        std::shared_ptr<Texture> m_texture;
    };

    /// <summary>
    /// Manages texture resources with LRU caching.
    /// When cache exceeds maxCacheSize, least-recently-used textures are evicted
    /// (unless pinned via PreloadHint).
    /// </summary>
    class TextureManager {
    public:
        static constexpr size_t DEFAULT_MAX_CACHE = 256 * 1024 * 1024; // 256 MB

        TextureManager(IRenderer* renderer, ILogger* logger = nullptr);
        ~TextureManager();

        // Load texture (returns cached if already loaded)
        std::shared_ptr<Texture> LoadTexture(const std::string& name, const std::string& path);

        // Get cached texture (returns nullptr if not loaded)
        std::shared_ptr<Texture> GetTexture(const std::string& name);

        // Check if texture is loaded
        bool IsLoaded(const std::string& name) const;

        // Unload specific texture
        void UnloadTexture(const std::string& name);

        // Unload all textures
        void UnloadAll();

        // Get statistics
        size_t GetLoadedCount() const { return m_textures.size(); }
        size_t GetTotalMemoryUsage() const;

        // Preload multiple textures
        void PreloadTextures(const std::vector<std::pair<std::string, std::string>>& textures);

        // Cache size limit
        void SetMaxCacheSize(size_t bytes) { m_maxCacheSize = bytes; }
        size_t GetMaxCacheSize() const { return m_maxCacheSize; }

        // Pin a texture so it won't be evicted by LRU
        void Pin(const std::string& name);
        void Unpin(const std::string& name);

    private:
        void TouchLRU(const std::string& name);
        void EvictIfNeeded();

        IRenderer* m_renderer;
        ILogger* m_logger;
        std::unordered_map<std::string, std::shared_ptr<TextureResource>> m_textures;

        // LRU order: front = most recently used, back = least recently used
        std::list<std::string> m_lruOrder;
        std::unordered_map<std::string, std::list<std::string>::iterator> m_lruMap;

        // Pinned textures (exempt from eviction)
        std::unordered_set<std::string> m_pinned;

        size_t m_maxCacheSize;
    };

} // namespace Resources
} // namespace Engine
