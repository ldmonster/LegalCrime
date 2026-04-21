#pragma once

#include "IMusicPlayer.h"
#include "ISoundPlayer.h"
#include "../Core/Types.h"
#include <string>
#include <vector>
#include <memory>

namespace Engine {

    class ILogger;
    class SoundEffect;
    class IMusicPlayer;
    class ISoundPlayer;

    /// Aggregated audio lifecycle interface.
    /// Focused playback capabilities are exposed via GetMusicPlayer()/GetSoundPlayer().
    class IAudioEngine {
    public:
        virtual ~IAudioEngine() = default;

        virtual Result<void> Initialize() = 0;
        virtual void Shutdown() = 0;

        // Volume control
        virtual void SetVolume(float volume) = 0;
        virtual float GetVolume() const = 0;

        virtual bool IsInitialized() const = 0;

        virtual IMusicPlayer& GetMusicPlayer() = 0;
        virtual ISoundPlayer& GetSoundPlayer() = 0;
    };
}
