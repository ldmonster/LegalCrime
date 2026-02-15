#include "AudioManager.h"

namespace Engine {
namespace Resources {

    AudioManager::AudioManager(IAudioEngine* audioEngine, ILogger* logger)
        : m_audioEngine(audioEngine)
        , m_logger(logger) {
        if (m_logger) {
            m_logger->Info("AudioManager initialized");
        }
    }

    AudioManager::~AudioManager() {
        UnloadAll();
        if (m_logger) {
            m_logger->Info("AudioManager destroyed");
        }
    }

    std::shared_ptr<SoundEffect> AudioManager::LoadSound(const std::string& name, const std::string& path) {
        // Check if already loaded
        auto it = m_sounds.find(name);
        if (it != m_sounds.end()) {
            if (m_logger) {
                m_logger->Debug("Sound '" + name + "' found in cache");
            }
            return it->second->GetSound();
        }

        // Load new sound
        if (!m_audioEngine || !m_audioEngine->IsInitialized()) {
            if (m_logger) {
                m_logger->Warning("Cannot load sound: AudioEngine not initialized");
            }
            return nullptr;
        }

        if (m_logger) {
            m_logger->Info("Loading sound: " + name + " from " + path);
        }

        auto sound = m_audioEngine->LoadSoundEffect(path);
        if (!sound) {
            if (m_logger) {
                m_logger->Error("Failed to load sound: " + path);
            }
            return nullptr;
        }

        // Cache the sound
        auto soundResource = std::make_shared<SoundResource>(name, path, sound);
        m_sounds[name] = soundResource;

        if (m_logger) {
            m_logger->Info("Sound loaded successfully: " + name);
        }

        return sound;
    }

    std::shared_ptr<SoundEffect> AudioManager::GetSound(const std::string& name) {
        auto it = m_sounds.find(name);
        if (it != m_sounds.end()) {
            return it->second->GetSound();
        }

        if (m_logger) {
            m_logger->Warning("Sound not found: " + name);
        }
        return nullptr;
    }

    void AudioManager::UnloadSound(const std::string& name) {
        auto it = m_sounds.find(name);
        if (it != m_sounds.end()) {
            if (m_logger) {
                m_logger->Info("Unloading sound: " + name);
            }
            m_sounds.erase(it);
        }
    }

    bool AudioManager::RegisterMusic(const std::string& name, const std::string& path) {
        // Check if already registered
        if (m_music.find(name) != m_music.end()) {
            if (m_logger) {
                m_logger->Debug("Music '" + name + "' already registered");
            }
            return true;
        }

        if (m_logger) {
            m_logger->Info("Registering music: " + name + " -> " + path);
        }

        auto musicResource = std::make_shared<MusicResource>(name, path);
        m_music[name] = musicResource;
        return true;
    }

    const std::string& AudioManager::GetMusicPath(const std::string& name) {
        static const std::string emptyString;
        
        auto it = m_music.find(name);
        if (it != m_music.end()) {
            return it->second->GetPath();
        }

        if (m_logger) {
            m_logger->Warning("Music not found: " + name);
        }
        return emptyString;
    }

    void AudioManager::UnloadMusic(const std::string& name) {
        auto it = m_music.find(name);
        if (it != m_music.end()) {
            if (m_logger) {
                m_logger->Info("Unregistering music: " + name);
            }
            m_music.erase(it);
        }
    }

    void AudioManager::PlaySound(const std::string& name, int loops) {
        auto sound = GetSound(name);
        if (sound && m_audioEngine) {
            sound->Play(loops);
        }
    }

    void AudioManager::PlayMusic(const std::string& name, int loops) {
        const std::string& path = GetMusicPath(name);
        if (!path.empty() && m_audioEngine) {
            // Load music from directory containing this track
            // Note: Current API loads from directory, not individual file
            // This is a limitation of the current design
            size_t lastSlash = path.find_last_of("/\\");
            std::string directory = (lastSlash != std::string::npos) ? path.substr(0, lastSlash) : ".";

            m_audioEngine->LoadMusicFromDirectory(directory);
            m_audioEngine->PlayMusic();
        }
    }

    void AudioManager::UnloadAll() {
        if (m_logger) {
            if (!m_sounds.empty() || !m_music.empty()) {
                m_logger->Info("Unloading all audio resources");
            }
        }
        m_sounds.clear();
        m_music.clear();
    }

} // namespace Resources
} // namespace Engine
