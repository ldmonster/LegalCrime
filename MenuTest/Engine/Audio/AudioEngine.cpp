#include "AudioEngine.h"
#include <SDL3/SDL.h>
#include <algorithm>

namespace Engine {

    AudioEngine::AudioEngine(ILogger* logger)
        : m_logger(logger)
        , m_mixer(nullptr)
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

        // Create focused subsystem for music playback (SRP)
        m_musicPlayer = std::make_unique<MusicPlayer>(m_mixer, m_logger);

        m_initialized = true;

        if (m_logger) {
            m_logger->Debug("Audio engine initialized successfully");
        }

        return Result<void>::Success();
    }

    void AudioEngine::Shutdown() {
        m_musicPlayer.reset();

        if (m_mixer) {
            MIX_DestroyMixer(m_mixer);
            m_mixer = nullptr;
        }

        MIX_Quit();
        m_initialized = false;

        if (m_logger) {
            m_logger->Debug("Audio engine shutdown");
        }
    }

    // Music delegation
    Result<void> AudioEngine::LoadMusicFromDirectory(const std::string& directory) {
        if (!m_musicPlayer) return Result<void>::Failure("Music player not initialized");
        return m_musicPlayer->LoadMusicFromDirectory(directory);
    }
    Result<void> AudioEngine::PlayMusic() {
        if (!m_musicPlayer) return Result<void>::Failure("Music player not initialized");
        return m_musicPlayer->PlayMusic();
    }
    Result<void> AudioEngine::StopMusic() {
        if (!m_musicPlayer) return Result<void>::Failure("Music player not initialized");
        return m_musicPlayer->StopMusic();
    }
    Result<void> AudioEngine::PauseMusic() {
        if (!m_musicPlayer) return Result<void>::Failure("Music player not initialized");
        return m_musicPlayer->PauseMusic();
    }
    Result<void> AudioEngine::ResumeMusic() {
        if (!m_musicPlayer) return Result<void>::Failure("Music player not initialized");
        return m_musicPlayer->ResumeMusic();
    }
    void AudioEngine::NextTrack() { if (m_musicPlayer) m_musicPlayer->NextTrack(); }
    void AudioEngine::PreviousTrack() { if (m_musicPlayer) m_musicPlayer->PreviousTrack(); }
    void AudioEngine::RandomTrack() { if (m_musicPlayer) m_musicPlayer->RandomTrack(); }
    bool AudioEngine::IsPlaying() const { return m_musicPlayer ? m_musicPlayer->IsPlaying() : false; }

    // Volume
    void AudioEngine::SetVolume(float volume) {
        m_volume = std::max(0.0f, std::min(1.0f, volume));
    }

    // Sound effects
    std::shared_ptr<SoundEffect> AudioEngine::LoadSoundEffect(const std::string& path) {
        if (!m_initialized) {
            if (m_logger) {
                m_logger->Error("Cannot load sound effect: AudioEngine not initialized");
            }
            return nullptr;
        }
        return SoundEffect::LoadFromFile(path, m_mixer, m_logger);
    }

    Result<void> AudioEngine::PlaySoundEffect(const std::shared_ptr<SoundEffect>& sound, int loops) {
        if (!m_initialized) {
            return Result<void>::Failure("AudioEngine not initialized");
        }
        if (!sound || !sound->IsValid()) {
            return Result<void>::Failure("Invalid sound effect");
        }
        return sound->Play(loops);
    }
}
