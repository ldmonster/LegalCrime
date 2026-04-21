#pragma once

#include "Types.h"
#include "Logger/ILogger.h"
#include "EngineBuilder.h"
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
        Result<void> Initialize(EngineSubsystems subsystems);
        void Shutdown();
        
        int Run();
        void Quit();
        
        // Subsystem accessors
        ILogger* GetLogger() const { return m_logger; }
        IWindow* GetWindow() const { return m_window.get(); }
        IRenderer* GetRenderer() const { return m_renderer.get(); }
        IAudioEngine* GetAudioEngine() const { return m_audioEngine.get(); }
        SceneManager* GetSceneManager() const { return m_sceneManager.get(); }
        Input::InputManager* GetInputManager() const { return m_inputManager.get(); }
        Resources::ResourceManager* GetResourceManager() const { return m_resourceManager.get(); }

        // Testable fixed-timestep helper.
        static int ComputeFixedUpdateCount(
            float frameTime,
            float fixedDt,
            float maxFrameTime,
            int maxUpdatesPerFrame,
            float& accumulator
        );
        
    protected:
        // Override these in derived game class
        virtual Result<void> OnInitialize() { return Result<void>::Success(); }
        virtual void OnShutdown() {}
        virtual void OnUpdate(float deltaTime) {}

        float GetInterpolationAlpha() const { return m_interpolationAlpha; }
        
    private:
        ILogger* m_logger;  // Raw pointer - managed manually to ensure it outlives all subsystems
        std::unique_ptr<IWindow> m_window;
        std::unique_ptr<IRenderer> m_renderer;
        std::unique_ptr<IAudioEngine> m_audioEngine;
        std::unique_ptr<SceneManager> m_sceneManager;
        std::unique_ptr<Input::InputManager> m_inputManager;
        std::unique_ptr<Resources::ResourceManager> m_resourceManager;

        bool m_initialized;
        bool m_running;
        
        uint64 m_lastFrameTime;
        float m_fixedAccumulator;
        float m_interpolationAlpha;

        static constexpr float FIXED_DT = 1.0f / 60.0f;
        static constexpr float MAX_FRAME_TIME = 0.1f;
        static constexpr int MAX_UPDATES_PER_FRAME = 5;
        
        void ProcessEvents();
        void Update(float deltaTime);
        void Render();
        
        float CalculateDeltaTime();
    };
}
