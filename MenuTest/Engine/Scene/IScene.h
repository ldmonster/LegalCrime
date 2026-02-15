#pragma once

#include "../Core/Types.h"
#include <SDL3/SDL.h>
#include <string>

namespace Engine {
    
    class IRenderer;
    class ILogger;
    
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
    };
}
