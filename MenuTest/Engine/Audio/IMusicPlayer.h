#pragma once

#include "../Core/Types.h"
#include <string>

namespace Engine {

    /// Music playback interface (ISP — streaming music only).
    class IMusicPlayer {
    public:
        virtual ~IMusicPlayer() = default;

        virtual bool LoadMusicFromDirectory(const std::string& directory) = 0;
        virtual bool PlayMusic() = 0;
        virtual bool StopMusic() = 0;
        virtual bool PauseMusic() = 0;
        virtual bool ResumeMusic() = 0;

        virtual void NextTrack() = 0;
        virtual void PreviousTrack() = 0;
        virtual void RandomTrack() = 0;

        virtual bool IsPlaying() const = 0;
    };

} // namespace Engine
