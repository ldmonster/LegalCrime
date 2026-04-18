#pragma once

#include "../Core/Types.h"
#include <string>

namespace Engine {

    /// Music playback interface (ISP — streaming music only).
    class IMusicPlayer {
    public:
        virtual ~IMusicPlayer() = default;

        virtual Result<void> LoadMusicFromDirectory(const std::string& directory) = 0;
        virtual Result<void> PlayMusic() = 0;
        virtual Result<void> StopMusic() = 0;
        virtual Result<void> PauseMusic() = 0;
        virtual Result<void> ResumeMusic() = 0;

        virtual void NextTrack() = 0;
        virtual void PreviousTrack() = 0;
        virtual void RandomTrack() = 0;

        virtual bool IsPlaying() const = 0;
    };

} // namespace Engine
