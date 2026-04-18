#pragma once

#include "IAudioEngine.h"
#include "MusicPlayer.h"
#include "SoundEffect.h"
#include "../Core/Logger/ILogger.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <vector>
#include <string>
#include <memory>

namespace Engine {

    /// AudioEngine — owns SDL_mixer lifecycle and delegates to focused subsystems.
    /// Music playback → MusicPlayer (SRP).
    /// Sound effects → played directly via SoundEffect::Play().
    class AudioEngine : public IAudioEngine {
    public:
        explicit AudioEngine(ILogger* logger);
        ~AudioEngine() override;

        Result<void> Initialize() override;
        void Shutdown() override;

        // Music playback (delegates to MusicPlayer)
        Result<void> LoadMusicFromDirectory(const std::string& directory) override;
        Result<void> PlayMusic() override;
        Result<void> StopMusic() override;
        Result<void> PauseMusic() override;
        Result<void> ResumeMusic() override;

        void NextTrack() override;
        void PreviousTrack() override;
        void RandomTrack() override;

        // Sound effects
        std::shared_ptr<SoundEffect> LoadSoundEffect(const std::string& path) override;
        Result<void> PlaySoundEffect(const std::shared_ptr<SoundEffect>& sound, int loops = 0) override;

        // Volume control
        void SetVolume(float volume) override;
        float GetVolume() const override { return m_volume; }

        bool IsPlaying() const override;
        bool IsInitialized() const override { return m_initialized; }

        // Access focused subsystems
        MusicPlayer* GetMusicPlayer() { return m_musicPlayer.get(); }

    private:
        ILogger* m_logger;
        MIX_Mixer* m_mixer;
        float m_volume;
        bool m_initialized;

        std::unique_ptr<MusicPlayer> m_musicPlayer;
    };
}
