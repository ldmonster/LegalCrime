#pragma once

#include "IScene.h"
#include "../Core/Types.h"
#include <memory>
#include <string>
#include <vector>

namespace Engine {
    
    class ILogger;
    
    enum class SceneTransition {
        None,
        Push,      // Add new scene on top of current
        Pop,       // Remove current scene
        Replace    // Replace current scene with new one
    };
    
    struct SceneCommand {
        SceneTransition transition;
        std::unique_ptr<IScene> newScene;
        
        SceneCommand() : transition(SceneTransition::None), newScene(nullptr) {}
        
        static SceneCommand Push(std::unique_ptr<IScene> scene) {
            SceneCommand cmd;
            cmd.transition = SceneTransition::Push;
            cmd.newScene = std::move(scene);
            return cmd;
        }
        
        static SceneCommand Pop() {
            SceneCommand cmd;
            cmd.transition = SceneTransition::Pop;
            return cmd;
        }
        
        static SceneCommand Replace(std::unique_ptr<IScene> scene) {
            SceneCommand cmd;
            cmd.transition = SceneTransition::Replace;
            cmd.newScene = std::move(scene);
            return cmd;
        }
    };
    
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
        
        void ProcessSceneCommand(SceneCommand& command);
    };
}
