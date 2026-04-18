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

    /// Combined audio interface — extends IMusicPlayer + ISoundPlayer.
    /// Consumers should prefer the focused interface they actually need.
    class IAudioEngine : public IMusicPlayer, public ISoundPlayer {
    public:
        virtual ~IAudioEngine() = default;

        virtual Result<void> Initialize() = 0;
        virtual void Shutdown() = 0;

        // Volume control
        virtual void SetVolume(float volume) = 0;
        virtual float GetVolume() const = 0;

        virtual bool IsInitialized() const = 0;
    };
}
