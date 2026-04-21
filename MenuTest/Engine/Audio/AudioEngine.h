#pragma once

#include "IAudioEngine.h"
#include "ISoundPlayer.h"
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
    class AudioEngine : public IAudioEngine, public ISoundPlayer {
    public:
        explicit AudioEngine(ILogger* logger);
        ~AudioEngine() override;

        Result<void> Initialize() override;
        void Shutdown() override;

        // Sound effects (ISoundPlayer)
        std::shared_ptr<SoundEffect> LoadSoundEffect(const std::string& path) override;
        Result<void> PlaySoundEffect(const std::shared_ptr<SoundEffect>& sound, int loops = 0) override;

        // Volume control
        void SetVolume(float volume) override;
        float GetVolume() const override { return m_volume; }

        bool IsInitialized() const override { return m_initialized; }

        // Focused interface access (IAudioEngine)
        IMusicPlayer& GetMusicPlayer() override { return *m_musicPlayer; }
        ISoundPlayer& GetSoundPlayer() override { return *this; }

        // Backward-compatible music helpers on concrete type.
        Result<void> LoadMusicFromDirectory(const std::string& directory);
        Result<void> PlayMusic();
        Result<void> StopMusic();
        Result<void> PauseMusic();
        Result<void> ResumeMusic();
        void NextTrack();
        void PreviousTrack();
        void RandomTrack();
        bool IsPlaying() const;

    private:
        ILogger* m_logger;
        MIX_Mixer* m_mixer;
        float m_volume;
        bool m_initialized;

        std::unique_ptr<MusicPlayer> m_musicPlayer;
    };
}
