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

    struct ApplicationConfig;

    /// Holds all engine subsystems. Produced by EngineBuilder.
    /// Application takes ownership of these via move semantics.
    struct EngineSubsystems {
        ILogger* logger = nullptr;          // Raw pointer — caller must manage lifetime
        std::unique_ptr<IWindow> window;
        std::unique_ptr<IRenderer> renderer;
        std::unique_ptr<IAudioEngine> audioEngine;
        std::unique_ptr<Input::InputManager> inputManager;
        std::unique_ptr<Resources::ResourceManager> resourceManager;
        std::unique_ptr<SceneManager> sceneManager;
    };

    /// Builds engine subsystems from an ApplicationConfig.
    /// Allows overriding individual subsystems for testing.
    class EngineBuilder {
    public:
        EngineBuilder() = default;

        /// Override the logger (default: new Logger).
        EngineBuilder& WithLogger(ILogger* logger);

        /// Override the window (default: new Window).
        EngineBuilder& WithWindow(std::unique_ptr<IWindow> window);

        /// Override the renderer (default: new Renderer).
        EngineBuilder& WithRenderer(std::unique_ptr<IRenderer> renderer);

        /// Override the audio engine (default: new AudioEngine).
        EngineBuilder& WithAudioEngine(std::unique_ptr<IAudioEngine> audioEngine);

        /// Override the input manager (default: new InputManager).
        EngineBuilder& WithInputManager(std::unique_ptr<Input::InputManager> inputManager);

        /// Build all subsystems. Initializes SDL if needed.
        /// Returns failure if any critical subsystem fails to initialize.
        Result<EngineSubsystems> Build(const ApplicationConfig& config);

    private:
        ILogger* m_overrideLogger = nullptr;
        std::unique_ptr<IWindow> m_overrideWindow;
        std::unique_ptr<IRenderer> m_overrideRenderer;
        std::unique_ptr<IAudioEngine> m_overrideAudioEngine;
        std::unique_ptr<Input::InputManager> m_overrideInputManager;
    };

} // namespace Engine
