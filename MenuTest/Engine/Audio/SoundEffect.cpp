#include "SoundEffect.h"
#include "../Core/Logger/ILogger.h"
#include <SDL3/SDL.h>

namespace Engine {

    SoundEffect::SoundEffect(MIX_Audio* audio, MIX_Mixer* mixer, const std::string& path)
        : m_audio(audio)
        , m_mixer(mixer)
        , m_path(path) {
    }
    
    SoundEffect::~SoundEffect() {
        if (m_audio) {
            MIX_DestroyAudio(m_audio);
            m_audio = nullptr;
        }
    }
    
    SoundEffect::SoundEffect(SoundEffect&& other) noexcept
        : m_audio(other.m_audio)
        , m_mixer(other.m_mixer)
        , m_path(std::move(other.m_path)) {
        other.m_audio = nullptr;
        other.m_mixer = nullptr;
    }
    
    SoundEffect& SoundEffect::operator=(SoundEffect&& other) noexcept {
        if (this != &other) {
            if (m_audio) {
                MIX_DestroyAudio(m_audio);
            }

            m_audio = other.m_audio;
            m_mixer = other.m_mixer;
            m_path = std::move(other.m_path);

            other.m_audio = nullptr;
            other.m_mixer = nullptr;
        }
        return *this;
    }
    
    std::shared_ptr<SoundEffect> SoundEffect::LoadFromFile(
        const std::string& path,
        MIX_Mixer* mixer,
        ILogger* logger
    ) {
        if (!mixer) {
            if (logger) {
                logger->Error("Cannot load sound effect: mixer is null");
            }
            return nullptr;
        }

        SDL_IOStream* rw = SDL_IOFromFile(path.c_str(), "rb");
        if (!rw) {
            if (logger) {
                logger->Error("Failed to open sound file: " + path + " - " + SDL_GetError());
            }
            return nullptr;
        }

        MIX_Audio* audio = MIX_LoadAudio_IO(mixer, rw, true, false);
        if (!audio) {
            if (logger) {
                logger->Error("Failed to load sound effect: " + path + " - " + SDL_GetError());
            }
            return nullptr;
        }

        if (logger) {
            logger->Debug("Loaded sound effect: " + path);
        }

        return std::shared_ptr<SoundEffect>(new SoundEffect(audio, mixer, path));
    }
    
    bool SoundEffect::Play(int loops) {
        if (!m_audio || !m_mixer) {
            return false;
        }

        // Play sound effect on the mixer
        bool result = MIX_PlayAudio(m_mixer, m_audio);
        return result;
    }
}
