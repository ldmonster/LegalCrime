#pragma once

#include "../Core/Types.h"
#include <SDL3/SDL.h>
#include <string>

namespace Engine {
    
    class IRenderer;
    class ILogger;

    /// Scene identity tag — avoids dynamic_cast in hot loops.
    enum class SceneId : uint8_t {
        Unknown = 0,
        MainMenu,
        Gameplay,
        // Extend as needed
    };
    
    // Abstract scene interface
    class IScene {
    public:
        virtual ~IScene() = default;
        
        virtual Result<void> Initialize() = 0;
        virtual void Shutdown() = 0;
        
        virtual void HandleEvent(const SDL_Event& event) = 0;
        virtual void Update(float deltaTime) = 0;
        virtual void Render() = 0;
        
        virtual const std::string& GetName() const = 0;
        virtual bool IsInitialized() const = 0;

        /// Return a compile-time scene identifier (avoids RTTI).
        virtual SceneId GetSceneId() const { return SceneId::Unknown; }
    };
}
