#include "AudioEngine.h"
#include <SDL3/SDL.h>
#include <filesystem>
#include <algorithm>
#include <random>
#include <cctype>

namespace Engine {
    
    AudioEngine::AudioEngine(ILogger* logger)
        : m_logger(logger)
        , m_mixer(nullptr)
        , m_track(nullptr)
        , m_currentAudio(nullptr)
        , m_currentFile(nullptr)
        , m_currentTrackIndex(0)
        , m_volume(1.0f)
        , m_initialized(false) {
    }
    
    AudioEngine::~AudioEngine() {
        Shutdown();
    }
    
    Result<void> AudioEngine::Initialize() {
        if (m_initialized) {
            return Result<void>::Failure("Audio engine already initialized");
        }
        
        if (!MIX_Init()) {
            std::string error = "SDL_mixer could not initialize! SDL Error: ";
            error += SDL_GetError();
            if (m_logger) m_logger->Error(error);
            return Result<void>::Failure(error);
        }
        
        m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (!m_mixer) {
            std::string error = "Failed to create mixer device! SDL Error: ";
            error += SDL_GetError();
            if (m_logger) m_logger->Error(error);
            MIX_Quit();
            return Result<void>::Failure(error);
        }
        
        m_track = MIX_CreateTrack(m_mixer);
        if (!m_track) {
            std::string error = "Failed to create audio track! SDL Error: ";
            error += SDL_GetError();
            if (m_logger) m_logger->Error(error);
            MIX_DestroyMixer(m_mixer);
            MIX_Quit();
            return Result<void>::Failure(error);
        }

        // Set callback for when track finishes
        // MIX_SetTrackStoppedCallback(m_track, OnTrackStopped, this);

        m_initialized = true;

        if (m_logger) {
            m_logger->Debug("Audio engine initialized successfully");
        }

        return Result<void>::Success();
    }
    
    void AudioEngine::Shutdown() {
        StopMusic();
        CleanupCurrentTrack();
        
        if (m_track) {
            MIX_DestroyTrack(m_track);
            m_track = nullptr;
        }
        
        if (m_mixer) {
            MIX_DestroyMixer(m_mixer);
            m_mixer = nullptr;
        }
        
        MIX_Quit();
        
        m_musicPaths.clear();
        m_initialized = false;
        
        if (m_logger) {
            m_logger->Debug("Audio engine shutdown");
        }
    }
    
    bool AudioEngine::LoadMusicFromDirectory(const std::string& directory) {
        if (!m_initialized) {
            if (m_logger) m_logger->Warning("Cannot load music: Audio engine not initialized");
            return false;
        }
        
        m_musicPaths.clear();
        
        try {
            namespace fs = std::filesystem;
            
            if (!fs::exists(directory) || !fs::is_directory(directory)) {
                if (m_logger) {
                    m_logger->Warning("Music directory does not exist: " + directory);
                }
                return false;
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
            
            return !m_musicPaths.empty();
            
        } catch (const std::exception& e) {
            if (m_logger) {
                m_logger->Error("Error loading music directory: " + std::string(e.what()));
            }
            return false;
        }
    }
    
    bool AudioEngine::PlayMusic() {
        if (!m_initialized || m_musicPaths.empty()) {
            return false;
        }
        
        return LoadAndPlayTrack(m_currentTrackIndex);
    }
    
    bool AudioEngine::StopMusic() {
        if (!m_initialized || !m_track) {
            return false;
        }
        
        MIX_StopTrack(m_track, 0);
        CleanupCurrentTrack();
        return true;
    }
    
    bool AudioEngine::PauseMusic() {
        if (!m_initialized || !m_track) {
            return false;
        }
        
        MIX_PauseTrack(m_track);
        return true;
    }
    
    bool AudioEngine::ResumeMusic() {
        if (!m_initialized || !m_track) {
            return false;
        }
        
        MIX_ResumeTrack(m_track);
        return true;
    }
    
    void AudioEngine::NextTrack() {
        if (m_musicPaths.empty()) return;
        
        StopMusic();
        m_currentTrackIndex = (m_currentTrackIndex + 1) % m_musicPaths.size();
        PlayMusic();
    }
    
    void AudioEngine::PreviousTrack() {
        if (m_musicPaths.empty()) return;
        
        StopMusic();
        m_currentTrackIndex = (m_currentTrackIndex == 0) 
            ? m_musicPaths.size() - 1 
            : m_currentTrackIndex - 1;
        PlayMusic();
    }
    
    void AudioEngine::RandomTrack() {
        if (m_musicPaths.empty()) return;
        
        StopMusic();
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, static_cast<int>(m_musicPaths.size() - 1));
        
        m_currentTrackIndex = dis(gen);
        PlayMusic();
    }
    
    void AudioEngine::SetVolume(float volume) {
        m_volume = std::max(0.0f, std::min(1.0f, volume));

        if (m_track) {
            // SDL3_mixer volume API may vary - commenting out for now
            // MIX_SetTrackVolume(m_track, m_volume);
        }
    }
    
    bool AudioEngine::IsPlaying() const {
        if (!m_initialized || !m_track) {
            return false;
        }
        
        return MIX_TrackPlaying(m_track) != 0;
    }
    
    bool AudioEngine::LoadAndPlayTrack(size_t index) {
        if (index >= m_musicPaths.size()) {
            return false;
        }
        
        CleanupCurrentTrack();
        
        const std::string& path = m_musicPaths[index];
        
        m_currentFile = SDL_IOFromFile(path.c_str(), "rb");
        if (!m_currentFile) {
            if (m_logger) {
                std::string error = "Failed to load music file: " + path + " - " + SDL_GetError();
                m_logger->Error(error);
            }
            return false;
        }
        
        m_currentAudio = MIX_LoadAudio_IO(m_mixer, m_currentFile, true, false);
        if (!m_currentAudio) {
            if (m_logger) {
                std::string error = "Failed to decode music: " + path + " - " + SDL_GetError();
                m_logger->Error(error);
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
            m_logger->Debug("Playing music: " + path);
        }
        
        return true;
    }
    
    void AudioEngine::CleanupCurrentTrack() {
        if (m_currentAudio) {
            MIX_DestroyAudio(m_currentAudio);
            m_currentAudio = nullptr;
        }
        
        if (m_currentFile) {
            SDL_CloseIO(m_currentFile);
            m_currentFile = nullptr;
        }
    }
    
    void AudioEngine::OnTrackStopped(void* userdata, MIX_Track* track) {
        AudioEngine* engine = static_cast<AudioEngine*>(userdata);
        if (engine) {
            engine->NextTrack();
        }
    }

    std::shared_ptr<SoundEffect> AudioEngine::LoadSoundEffect(const std::string& path) {
        if (!m_initialized) {
            if (m_logger) {
                m_logger->Error("Cannot load sound effect: AudioEngine not initialized");
            }
            return nullptr;
        }

        return SoundEffect::LoadFromFile(path, m_mixer, m_logger);
    }

    bool AudioEngine::PlaySoundEffect(const std::shared_ptr<SoundEffect>& sound, int loops) {
        if (!m_initialized) {
            if (m_logger) {
                m_logger->Error("Cannot play sound effect: AudioEngine not initialized");
            }
            return false;
        }

        if (!sound || !sound->IsValid()) {
            if (m_logger) {
                m_logger->Warning("Attempted to play invalid sound effect");
            }
            return false;
        }

        return sound->Play(loops);
    }
}
