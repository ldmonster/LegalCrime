#pragma once

#include "IMusicPlayer.h"
#include "../Core/Logger/ILogger.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <vector>
#include <string>
#include <mutex>

namespace Engine {

    /// Handles music playlist management and playback (SRP: music only).
    /// Extracted from AudioEngine to follow Single Responsibility Principle.
    class MusicPlayer : public IMusicPlayer {
    public:
        MusicPlayer(MIX_Mixer* mixer, ILogger* logger);
        ~MusicPlayer() override;

        // IMusicPlayer
        Result<void> LoadMusicFromDirectory(const std::string& directory) override;
        Result<void> PlayMusic() override;
        Result<void> StopMusic() override;
        Result<void> PauseMusic() override;
        Result<void> ResumeMusic() override;

        void NextTrack() override;
        void PreviousTrack() override;
        void RandomTrack() override;

        bool IsPlaying() const override;

    private:
        ILogger* m_logger;
        MIX_Mixer* m_mixer;
        MIX_Track* m_track;
        MIX_Audio* m_currentAudio;
        SDL_IOStream* m_currentFile;

        std::vector<std::string> m_musicPaths;
        size_t m_currentTrackIndex;
        mutable std::mutex m_mutex;

        bool LoadAndPlayTrack(size_t index);
        void CleanupCurrentTrack();
    };

} // namespace Engine
