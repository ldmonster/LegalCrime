#pragma once

#include "../Core/Types.h"
#include <string>
#include <memory>

namespace Engine {

    class SoundEffect;

    /// Sound effect playback interface (ISP — cached sound effects only).
    class ISoundPlayer {
    public:
        virtual ~ISoundPlayer() = default;

        virtual std::shared_ptr<SoundEffect> LoadSoundEffect(const std::string& path) = 0;
        virtual Result<void> PlaySoundEffect(const std::shared_ptr<SoundEffect>& sound, int loops = 0) = 0;
    };

} // namespace Engine
