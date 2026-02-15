#include "TextureManager.h"

namespace Engine {
namespace Resources {

    TextureManager::TextureManager(IRenderer* renderer, ILogger* logger)
        : m_renderer(renderer)
        , m_logger(logger) {
        if (m_logger) {
            m_logger->Info("TextureManager initialized");
        }
    }

    TextureManager::~TextureManager() {
        UnloadAll();
        if (m_logger) {
            m_logger->Info("TextureManager destroyed");
        }
    }

    std::shared_ptr<Texture> TextureManager::LoadTexture(const std::string& name, const std::string& path) {
        // Check if already loaded
        auto it = m_textures.find(name);
        if (it != m_textures.end()) {
            if (m_logger) {
                m_logger->Debug("Texture '" + name + "' found in cache");
            }
            return it->second->GetTexture();
        }

        // Load new texture
        if (m_logger) {
            m_logger->Info("Loading texture: " + name + " from " + path);
        }

        auto texture = Texture::LoadFromFile(m_renderer, path, m_logger);
        if (!texture) {
            if (m_logger) {
                m_logger->Error("Failed to load texture: " + path);
            }
            return nullptr;
        }

        // Cache the texture
        auto textureResource = std::make_shared<TextureResource>(name, path, texture);
        m_textures[name] = textureResource;

        if (m_logger) {
            m_logger->Info("Texture loaded successfully: " + name + 
                         " (" + std::to_string(texture->GetWidth()) + "x" + 
                         std::to_string(texture->GetHeight()) + ")");
        }

        return texture;
    }

    std::shared_ptr<Texture> TextureManager::GetTexture(const std::string& name) {
        auto it = m_textures.find(name);
        if (it != m_textures.end()) {
            return it->second->GetTexture();
        }
        
        if (m_logger) {
            m_logger->Warning("Texture not found: " + name);
        }
        return nullptr;
    }

    bool TextureManager::IsLoaded(const std::string& name) const {
        return m_textures.find(name) != m_textures.end();
    }

    void TextureManager::UnloadTexture(const std::string& name) {
        auto it = m_textures.find(name);
        if (it != m_textures.end()) {
            if (m_logger) {
                m_logger->Info("Unloading texture: " + name);
            }
            m_textures.erase(it);
        }
    }

    void TextureManager::UnloadAll() {
        if (m_logger && !m_textures.empty()) {
            m_logger->Info("Unloading all textures (" + std::to_string(m_textures.size()) + ")");
        }
        m_textures.clear();
    }

    size_t TextureManager::GetTotalMemoryUsage() const {
        size_t total = 0;
        for (const auto& pair : m_textures) {
            total += pair.second->GetSize();
        }
        return total;
    }

    void TextureManager::PreloadTextures(const std::vector<std::pair<std::string, std::string>>& textures) {
        if (m_logger) {
            m_logger->Info("Preloading " + std::to_string(textures.size()) + " textures...");
        }

        int successCount = 0;
        for (const auto& [name, path] : textures) {
            if (LoadTexture(name, path)) {
                successCount++;
            }
        }

        if (m_logger) {
            m_logger->Info("Preloaded " + std::to_string(successCount) + "/" + 
                         std::to_string(textures.size()) + " textures successfully");
        }
    }

} // namespace Resources
} // namespace Engine
