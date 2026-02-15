#pragma once

#include "IAudioEngine.h"
#include "SoundEffect.h"
#include "../Core/Logger/ILogger.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <vector>
#include <string>
#include <memory>

namespace Engine {

    class AudioEngine : public IAudioEngine {
    public:
        explicit AudioEngine(ILogger* logger);
        ~AudioEngine() override;

        Result<void> Initialize() override;
        void Shutdown() override;

        // Music playback
        bool LoadMusicFromDirectory(const std::string& directory) override;
        bool PlayMusic() override;
        bool StopMusic() override;
        bool PauseMusic() override;
        bool ResumeMusic() override;

        void NextTrack() override;
        void PreviousTrack() override;
        void RandomTrack() override;

        // Sound effects
        std::shared_ptr<SoundEffect> LoadSoundEffect(const std::string& path) override;
        bool PlaySoundEffect(const std::shared_ptr<SoundEffect>& sound, int loops = 0) override;

        // Volume control
        void SetVolume(float volume) override;
        float GetVolume() const override { return m_volume; }

        bool IsPlaying() const override;
        bool IsInitialized() const override { return m_initialized; }

    private:
        ILogger* m_logger;

        MIX_Mixer* m_mixer;
        MIX_Track* m_track;
        MIX_Audio* m_currentAudio;
        SDL_IOStream* m_currentFile;

        std::vector<std::string> m_musicPaths;
        size_t m_currentTrackIndex;

        float m_volume;
        bool m_initialized;

        bool LoadAndPlayTrack(size_t index);
        void CleanupCurrentTrack();

        // Static callback for track completion
        static void OnTrackStopped(void* userdata, MIX_Track* track);
    };
}
