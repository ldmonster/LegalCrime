#pragma once

#include "Types.h"
#include "Logger/ILogger.h"
#include "../Platform/IWindow.h"
#include "../Renderer/IRenderer.h"
#include "../Audio/IAudioEngine.h"
#include "../Scene/SceneManager.h"
#include "../Input/InputManager.h"
#include "../Resources/ResourceManager.h"
#include <memory>

namespace Engine {
    
    struct ApplicationConfig {
        std::string appName;
        WindowConfig windowConfig;
        RendererConfig rendererConfig;
        LogLevel logLevel;
        
        ApplicationConfig()
            : appName("Game Application")
            , logLevel(LogLevel::Info) {
        }
    };
    
    // Main application class - manages engine lifecycle
    class Application {
    public:
        Application();
        virtual ~Application();
        
        Result<void> Initialize(const ApplicationConfig& config);
        void Shutdown();
        
        int Run();
        void Quit();
        
        // Subsystem accessors
        ILogger* GetLogger() const { return m_logger.get(); }
        IWindow* GetWindow() const { return m_window.get(); }
        IRenderer* GetRenderer() const { return m_renderer.get(); }
        IAudioEngine* GetAudioEngine() const { return m_audioEngine.get(); }
        SceneManager* GetSceneManager() const { return m_sceneManager.get(); }
        Input::InputManager* GetInputManager() const { return m_inputManager.get(); }
        Resources::ResourceManager* GetResourceManager() const { return m_resourceManager.get(); }
        
    protected:
        // Override these in derived game class
        virtual Result<void> OnInitialize() { return Result<void>::Success(); }
        virtual void OnShutdown() {}
        virtual void OnUpdate(float deltaTime) {}
        
    private:
        std::unique_ptr<ILogger> m_logger;
        std::unique_ptr<IWindow> m_window;
        std::unique_ptr<IRenderer> m_renderer;
        std::unique_ptr<IAudioEngine> m_audioEngine;
        std::unique_ptr<SceneManager> m_sceneManager;
        std::unique_ptr<Input::InputManager> m_inputManager;
        std::unique_ptr<Resources::ResourceManager> m_resourceManager;

        bool m_initialized;
        bool m_running;
        
        uint64 m_lastFrameTime;
        
        void ProcessEvents();
        void Update(float deltaTime);
        void Render();
        
        float CalculateDeltaTime();
    };
}
