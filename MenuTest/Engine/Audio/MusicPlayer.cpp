#include "MusicPlayer.h"
#include <SDL3/SDL.h>
#include <filesystem>
#include <algorithm>
#include <random>
#include <cctype>

namespace Engine {

    MusicPlayer::MusicPlayer(MIX_Mixer* mixer, ILogger* logger)
        : m_logger(logger)
        , m_mixer(mixer)
        , m_track(nullptr)
        , m_currentAudio(nullptr)
        , m_currentFile(nullptr)
        , m_currentTrackIndex(0) {

        if (m_mixer) {
            m_track = MIX_CreateTrack(m_mixer);
            if (!m_track && m_logger) {
                m_logger->Error("MusicPlayer: Failed to create audio track: " + std::string(SDL_GetError()));
            }
        }
    }

    MusicPlayer::~MusicPlayer() {
        StopMusic();
        CleanupCurrentTrack();

        if (m_track) {
            MIX_DestroyTrack(m_track);
            m_track = nullptr;
        }
    }

    Result<void> MusicPlayer::LoadMusicFromDirectory(const std::string& directory) {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_musicPaths.clear();

        try {
            namespace fs = std::filesystem;

            if (!fs::exists(directory) || !fs::is_directory(directory)) {
                if (m_logger) {
                    m_logger->Warning("Music directory does not exist: " + directory);
                }
                return Result<void>::Failure("Music directory does not exist: " + directory);
            }

            for (const auto& entry : fs::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    std::string ext = entry.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                    if (ext == ".mp3" || ext == ".wav" || ext == ".ogg" || ext == ".flac") {
                        m_musicPaths.push_back(entry.path().string());
                    }
                }
            }

            if (m_logger) {
                m_logger->Info("Loaded " + std::to_string(m_musicPaths.size()) + " music files from: " + directory);
            }

            if (m_musicPaths.empty()) {
                return Result<void>::Failure("No music files found in: " + directory);
            }
            return Result<void>::Success();

        } catch (const std::exception& e) {
            if (m_logger) {
                m_logger->Error("Error loading music directory: " + std::string(e.what()));
            }
            return Result<void>::Failure("Error loading music directory: " + std::string(e.what()));
        }
    }

    Result<void> MusicPlayer::PlayMusic() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_musicPaths.empty()) return Result<void>::Failure("No music loaded");
        if (!LoadAndPlayTrack(m_currentTrackIndex)) {
            return Result<void>::Failure("Failed to play track");
        }
        return Result<void>::Success();
    }

    Result<void> MusicPlayer::StopMusic() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_track) return Result<void>::Failure("No track available");
        MIX_StopTrack(m_track, 0);
        CleanupCurrentTrack();
        return Result<void>::Success();
    }

    Result<void> MusicPlayer::PauseMusic() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_track) return Result<void>::Failure("No track available");
        MIX_PauseTrack(m_track);
        return Result<void>::Success();
    }

    Result<void> MusicPlayer::ResumeMusic() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_track) return Result<void>::Failure("No track available");
        MIX_ResumeTrack(m_track);
        return Result<void>::Success();
    }

    void MusicPlayer::NextTrack() {
        // StopMusic/PlayMusic each lock mutex, so we avoid double-locking
        StopMusic();
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_musicPaths.empty()) return;
            m_currentTrackIndex = (m_currentTrackIndex + 1) % m_musicPaths.size();
        }
        PlayMusic();
    }

    void MusicPlayer::PreviousTrack() {
        StopMusic();
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_musicPaths.empty()) return;
            m_currentTrackIndex = (m_currentTrackIndex == 0)
                ? m_musicPaths.size() - 1
                : m_currentTrackIndex - 1;
        }
        PlayMusic();
    }

    void MusicPlayer::RandomTrack() {
        StopMusic();
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_musicPaths.empty()) return;
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, static_cast<int>(m_musicPaths.size() - 1));
            m_currentTrackIndex = dis(gen);
        }
        PlayMusic();
    }

    bool MusicPlayer::IsPlaying() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_track) return false;
        return MIX_TrackPlaying(m_track) != 0;
    }

    bool MusicPlayer::LoadAndPlayTrack(size_t index) {
        // Caller must hold m_mutex
        if (index >= m_musicPaths.size()) return false;

        CleanupCurrentTrack();

        const std::string& path = m_musicPaths[index];

        m_currentFile = SDL_IOFromFile(path.c_str(), "rb");
        if (!m_currentFile) {
            if (m_logger) {
                m_logger->Error("Failed to load music file: " + path + " - " + SDL_GetError());
            }
            return false;
        }

        m_currentAudio = MIX_LoadAudio_IO(m_mixer, m_currentFile, true, true);
        if (!m_currentAudio) {
            if (m_logger) {
                m_logger->Error("Failed to decode music: " + path + " - " + SDL_GetError());
            }
            SDL_CloseIO(m_currentFile);
            m_currentFile = nullptr;
            return false;
        }

        MIX_SetTrackAudio(m_track, m_currentAudio);

        if (!MIX_TrackPlaying(m_track)) {
            MIX_PlayTrack(m_track, 0);
        }

        if (m_logger) {
            m_logger->Info("Playing music: " + path);
        }

        return true;
    }

    void MusicPlayer::CleanupCurrentTrack() {
        // Caller must hold m_mutex
        if (m_currentAudio) {
            MIX_DestroyAudio(m_currentAudio);
            m_currentAudio = nullptr;
        }
        m_currentFile = nullptr;
    }

} // namespace Engine
