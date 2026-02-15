#pragma once

#include "Resource.h"
#include "../Audio/SoundEffect.h"
#include "../Core/Logger/ILogger.h"
#include "../Audio/IAudioEngine.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace Engine {
namespace Resources {

    /// <summary>
    /// Sound resource wrapper.
    /// </summary>
    class SoundResource : public Resource {
    public:
        SoundResource(const std::string& name, const std::string& path, std::shared_ptr<SoundEffect> sound)
            : Resource(name, path)
            , m_sound(sound) {
            if (m_sound) {
                m_isLoaded = true;
                // Estimate size (can't get actual size from Mix_Chunk easily)
                m_sizeInBytes = 10240; // Placeholder
            }
        }

        std::shared_ptr<SoundEffect> GetSound() const { return m_sound; }

    protected:
        void Unload() override {
            m_sound.reset();
            Resource::Unload();
        }

    private:
        std::shared_ptr<SoundEffect> m_sound;
    };

    /// <summary>
    /// Music resource wrapper (path-based, since music is streamed).
    /// </summary>
    class MusicResource : public Resource {
    public:
        MusicResource(const std::string& name, const std::string& path)
            : Resource(name, path) {
            m_isLoaded = true;
        }
    };

    /// <summary>
    /// Manages audio resources (sounds and music) with caching.
    /// </summary>
    class AudioManager {
    public:
        AudioManager(IAudioEngine* audioEngine, ILogger* logger = nullptr);
        ~AudioManager();

        // Sound effects
        std::shared_ptr<SoundEffect> LoadSound(const std::string& name, const std::string& path);
        std::shared_ptr<SoundEffect> GetSound(const std::string& name);
        void UnloadSound(const std::string& name);

        // Music (path-based since it's streamed)
        bool RegisterMusic(const std::string& name, const std::string& path);
        const std::string& GetMusicPath(const std::string& name);
        void UnloadMusic(const std::string& name);

        // Playback helpers
        void PlaySound(const std::string& name, int loops = 0);
        void PlayMusic(const std::string& name, int loops = -1);

        // Cleanup
        void UnloadAll();

        // Statistics
        size_t GetLoadedSoundCount() const { return m_sounds.size(); }
        size_t GetRegisteredMusicCount() const { return m_music.size(); }

    private:
        IAudioEngine* m_audioEngine;
        ILogger* m_logger;
        std::unordered_map<std::string, std::shared_ptr<SoundResource>> m_sounds;
        std::unordered_map<std::string, std::shared_ptr<MusicResource>> m_music;
    };

} // namespace Resources
} // namespace Engine
