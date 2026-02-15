#pragma once

#include "../Core/Types.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <string>
#include <memory>

namespace Engine {

    class ILogger;

    // Represents a loaded sound effect
    class SoundEffect {
    public:
        // Factory method to load from file
        static std::shared_ptr<SoundEffect> LoadFromFile(
            const std::string& path,
            MIX_Mixer* mixer,
            ILogger* logger = nullptr
        );
        
        ~SoundEffect();
        
        // Disable copy, enable move
        SoundEffect(const SoundEffect&) = delete;
        SoundEffect& operator=(const SoundEffect&) = delete;
        SoundEffect(SoundEffect&& other) noexcept;
        SoundEffect& operator=(SoundEffect&& other) noexcept;
        
        // Play the sound effect
        bool Play(int loops = 0);
        
        // Get the underlying SDL audio handle
        MIX_Audio* GetHandle() const { return m_audio; }
        
        // Check if loaded successfully
        bool IsValid() const { return m_audio != nullptr; }

    private:
        SoundEffect(MIX_Audio* audio, MIX_Mixer* mixer, const std::string& path);

        MIX_Audio* m_audio;
        MIX_Mixer* m_mixer;
        std::string m_path;
    };
}
