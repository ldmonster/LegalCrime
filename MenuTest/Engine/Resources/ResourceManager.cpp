#include "ResourceManager.h"

namespace Engine {
namespace Resources {

    ResourceManager::ResourceManager(IRenderer* renderer, IAudioEngine* audioEngine, ILogger* logger)
        : m_logger(logger)
        , m_renderer(renderer)
        , m_audioEngine(audioEngine)
        , m_initialized(false) {
    }

    ResourceManager::~ResourceManager() {
        Shutdown();
    }

    void ResourceManager::Initialize() {
        if (m_initialized) {
            if (m_logger) {
                m_logger->Warning("ResourceManager already initialized");
            }
            return;
        }

        if (m_logger) {
            m_logger->Info("Initializing ResourceManager");
        }

        // Create sub-managers
        m_textureManager = std::make_unique<TextureManager>(m_renderer, m_logger);
        m_audioManager = std::make_unique<AudioManager>(m_audioEngine, m_logger);

        m_initialized = true;

        if (m_logger) {
            m_logger->Info("ResourceManager initialized successfully");
        }
    }

    void ResourceManager::Shutdown() {
        if (!m_initialized) {
            return;
        }

        if (m_logger) {
            m_logger->Info("Shutting down ResourceManager");
        }

        // Clean up all resources
        UnloadAll();

        // Destroy managers
        m_audioManager.reset();
        m_textureManager.reset();

        m_initialized = false;

        if (m_logger) {
            m_logger->Info("ResourceManager shutdown complete");
        }
    }

    void ResourceManager::LogResourceStats() {
        if (!m_logger) return;

        m_logger->Info("=== Resource Manager Statistics ===");
        
        // Texture stats
        size_t textureCount = m_textureManager->GetLoadedCount();
        size_t textureMemory = m_textureManager->GetTotalMemoryUsage();
        m_logger->Info("Textures: " + std::to_string(textureCount) + 
                      " loaded (" + std::to_string(textureMemory / 1024) + " KB)");

        // Audio stats
        size_t soundCount = m_audioManager->GetLoadedSoundCount();
        size_t musicCount = m_audioManager->GetRegisteredMusicCount();
        m_logger->Info("Sounds: " + std::to_string(soundCount) + " loaded");
        m_logger->Info("Music: " + std::to_string(musicCount) + " registered");

        m_logger->Info("===================================");
    }

    void ResourceManager::UnloadAll() {
        if (m_logger) {
            m_logger->Info("Unloading all resources");
        }

        if (m_audioManager) {
            m_audioManager->UnloadAll();
        }

        if (m_textureManager) {
            m_textureManager->UnloadAll();
        }
    }

} // namespace Resources
} // namespace Engine
