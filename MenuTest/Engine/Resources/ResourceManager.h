#pragma once

#include "TextureManager.h"
#include "AudioManager.h"
#include "../Core/Logger/ILogger.h"
#include "../Renderer/IRenderer.h"
#include "../Audio/IAudioEngine.h"
#include <memory>

namespace Engine {
namespace Resources {

    /// <summary>
    /// Central resource manager coordinating all resource types.
    /// Provides unified access to textures, audio, fonts, etc.
    /// </summary>
    class ResourceManager {
    public:
        ResourceManager(IRenderer* renderer, IAudioEngine* audioEngine, ILogger* logger = nullptr);
        ~ResourceManager();

        // Initialization
        void Initialize();
        void Shutdown();

        // Manager accessors
        TextureManager* GetTextureManager() { return m_textureManager.get(); }
        AudioManager* GetAudioManager() { return m_audioManager.get(); }

        // Convenience methods for common operations

        // Textures
        std::shared_ptr<Texture> LoadTexture(const std::string& name, const std::string& path) {
            if (!m_textureManager) return nullptr;
            return m_textureManager->LoadTexture(name, path);
        }

        std::shared_ptr<Texture> GetTexture(const std::string& name) {
            if (!m_textureManager) return nullptr;
            return m_textureManager->GetTexture(name);
        }

        // Sounds
        std::shared_ptr<SoundEffect> LoadSound(const std::string& name, const std::string& path) {
            if (!m_audioManager) return nullptr;
            return m_audioManager->LoadSound(name, path);
        }

        std::shared_ptr<SoundEffect> GetSound(const std::string& name) {
            if (!m_audioManager) return nullptr;
            return m_audioManager->GetSound(name);
        }

        void PlaySound(const std::string& name, int loops = 0) {
            if (!m_audioManager) return;
            m_audioManager->PlaySound(name, loops);
        }

        // Music
        Result<void> RegisterMusic(const std::string& name, const std::string& path) {
            if (!m_audioManager) return Result<void>::Failure("Audio manager not initialized");
            return m_audioManager->RegisterMusic(name, path);
        }

        void PlayMusic(const std::string& name, int loops = -1) {
            if (!m_audioManager) return;
            m_audioManager->PlayMusic(name, loops);
        }

        // Batch loading
        void PreloadTextures(const std::vector<std::pair<std::string, std::string>>& textures) {
            if (!m_textureManager) return;
            m_textureManager->PreloadTextures(textures);
        }

        // Statistics
        void LogResourceStats();
        size_t GetTotalTextureMemory() const {
            if (!m_textureManager) return 0;
            return m_textureManager->GetTotalMemoryUsage();
        }

        // Cleanup
        void UnloadAll();
        void UnloadTextures() { if (m_textureManager) m_textureManager->UnloadAll(); }
        void UnloadAudio() { if (m_audioManager) m_audioManager->UnloadAll(); }

    private:
        ILogger* m_logger;
        IRenderer* m_renderer;
        IAudioEngine* m_audioEngine;

        std::unique_ptr<TextureManager> m_textureManager;
        std::unique_ptr<AudioManager> m_audioManager;

        bool m_initialized;
    };

} // namespace Resources
} // namespace Engine
