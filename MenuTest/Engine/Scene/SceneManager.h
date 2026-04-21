#pragma once

#include "IScene.h"
#include "../Core/Types.h"
#include <memory>
#include <string>
#include <vector>

namespace Engine {
    
    class ILogger;
    
    // Manages scene stack and transitions
    class SceneManager {
    public:
        explicit SceneManager(ILogger* logger);
        ~SceneManager();
        
        void PushScene(std::unique_ptr<IScene> scene);
        void PopScene();
        void ReplaceScene(std::unique_ptr<IScene> scene);
        void ClearScenes();
        
        void HandleEvent(const SDL_Event& event);
        void Update(float deltaTime);
        void Render();
        
        IScene* GetCurrentScene() const;
        bool HasScenes() const;
        
    private:
        ILogger* m_logger;
        std::vector<std::unique_ptr<IScene>> m_sceneStack;
    };
}
