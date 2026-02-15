#pragma once

#include "../Core/Types.h"
#include <string>
#include <vector>
#include <memory>

namespace Engine {

    class ILogger;
    class SoundEffect;

    // Abstract audio interface
    class IAudioEngine {
    public:
        virtual ~IAudioEngine() = default;

        virtual Result<void> Initialize() = 0;
        virtual void Shutdown() = 0;

        // Music playback
        virtual bool LoadMusicFromDirectory(const std::string& directory) = 0;
        virtual bool PlayMusic() = 0;
        virtual bool StopMusic() = 0;
        virtual bool PauseMusic() = 0;
        virtual bool ResumeMusic() = 0;

        virtual void NextTrack() = 0;
        virtual void PreviousTrack() = 0;
        virtual void RandomTrack() = 0;

        // Sound effects
        virtual std::shared_ptr<SoundEffect> LoadSoundEffect(const std::string& path) = 0;
        virtual bool PlaySoundEffect(const std::shared_ptr<SoundEffect>& sound, int loops = 0) = 0;

        // Volume control
        virtual void SetVolume(float volume) = 0;
        virtual float GetVolume() const = 0;

        virtual bool IsPlaying() const = 0;
        virtual bool IsInitialized() const = 0;
    };
}
