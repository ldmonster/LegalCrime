#include "SceneManager.h"
#include "../Core/Logger/ILogger.h"

namespace Engine {
    
    SceneManager::SceneManager(ILogger* logger)
        : m_logger(logger) {
    }
    
    SceneManager::~SceneManager() {
        ClearScenes();
    }
    
    void SceneManager::PushScene(std::unique_ptr<IScene> scene) {
        if (!scene) {
            if (m_logger) m_logger->Warning("Cannot push null scene");
            return;
        }
        
        auto result = scene->Initialize();
        if (!result) {
            if (m_logger) {
                m_logger->Error("Failed to initialize scene: " + scene->GetName() + " - " + result.error);
            }
            return;
        }
        
        m_sceneStack.push_back(std::move(scene));
        
        if (m_logger) {
            m_logger->Debug("Pushed scene: " + m_sceneStack.back()->GetName());
        }
    }
    
    void SceneManager::PopScene() {
        if (m_sceneStack.empty()) {
            if (m_logger) m_logger->Warning("Cannot pop scene: Stack is empty");
            return;
        }
        
        auto& scene = m_sceneStack.back();
        if (m_logger) {
            m_logger->Debug("Popping scene: " + scene->GetName());
        }
        
        scene->Shutdown();
        m_sceneStack.pop_back();
    }
    
    void SceneManager::ReplaceScene(std::unique_ptr<IScene> scene) {
        if (!scene) {
            if (m_logger) m_logger->Warning("Cannot replace with null scene");
            return;
        }
        
        if (!m_sceneStack.empty()) {
            PopScene();
        }
        
        PushScene(std::move(scene));
    }
    
    void SceneManager::ClearScenes() {
        while (!m_sceneStack.empty()) {
            PopScene();
        }
    }
    
    void SceneManager::HandleEvent(const SDL_Event& event) {
        if (!m_sceneStack.empty()) {
            m_sceneStack.back()->HandleEvent(event);
        }
    }
    
    void SceneManager::Update(float deltaTime) {
        if (!m_sceneStack.empty()) {
            m_sceneStack.back()->Update(deltaTime);
        }
    }
    
    void SceneManager::Render() {
        if (!m_sceneStack.empty()) {
            m_sceneStack.back()->Render();
        }
    }
    
    IScene* SceneManager::GetCurrentScene() const {
        return m_sceneStack.empty() ? nullptr : m_sceneStack.back().get();
    }
    
    bool SceneManager::HasScenes() const {
        return !m_sceneStack.empty();
    }
}
