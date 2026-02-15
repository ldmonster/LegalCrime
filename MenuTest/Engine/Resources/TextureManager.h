#pragma once

#include "Resource.h"
#include "../Graphics/Texture.h"
#include "../Core/Logger/ILogger.h"
#include "../Renderer/IRenderer.h"
#include <memory>
#include <string>
#include <unordered_map>

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

    protected:
        void Unload() override {
            m_texture.reset();
            Resource::Unload();
        }

    private:
        std::shared_ptr<Texture> m_texture;
    };

    /// <summary>
    /// Manages texture resources with caching and ref-counting.
    /// </summary>
    class TextureManager {
    public:
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

    private:
        IRenderer* m_renderer;
        ILogger* m_logger;
        std::unordered_map<std::string, std::shared_ptr<TextureResource>> m_textures;
    };

} // namespace Resources
} // namespace Engine
