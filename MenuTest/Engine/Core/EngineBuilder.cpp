#include "EngineBuilder.h"
#include "Application.h"
#include "Logger/Logger.h"
#include "../Platform/Window.h"
#include "../Renderer/Renderer.h"
#include "../Audio/AudioEngine.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#define SDL_IMAGE_USE_SDL3 1
#include <SDL3_image/SDL_image.h>

namespace Engine {

    EngineBuilder& EngineBuilder::WithLogger(ILogger* logger) {
        m_overrideLogger = logger;
        return *this;
    }

    EngineBuilder& EngineBuilder::WithWindow(std::unique_ptr<IWindow> window) {
        m_overrideWindow = std::move(window);
        return *this;
    }

    EngineBuilder& EngineBuilder::WithRenderer(std::unique_ptr<IRenderer> renderer) {
        m_overrideRenderer = std::move(renderer);
        return *this;
    }

    EngineBuilder& EngineBuilder::WithAudioEngine(std::unique_ptr<IAudioEngine> audioEngine) {
        m_overrideAudioEngine = std::move(audioEngine);
        return *this;
    }

    EngineBuilder& EngineBuilder::WithInputManager(std::unique_ptr<Input::InputManager> inputManager) {
        m_overrideInputManager = std::move(inputManager);
        return *this;
    }

    Result<EngineSubsystems> EngineBuilder::Build(const ApplicationConfig& config) {
        EngineSubsystems subs;

        // Initialize SDL
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
            return Result<EngineSubsystems>::Failure(
                std::string("SDL initialization failed: ") + SDL_GetError());
        }

        // Initialize SDL_ttf
        if (!TTF_Init()) {
            SDL_Quit();
            return Result<EngineSubsystems>::Failure(
                std::string("SDL_ttf initialization failed: ") + SDL_GetError());
        }

        // Logger
        if (m_overrideLogger) {
            subs.logger = m_overrideLogger;
        } else {
            subs.logger = new Logger();
        }
        subs.logger->SetLogLevel(config.logLevel);
        subs.logger->Info("=== Building engine for " + config.appName + " ===");

        // Window
        if (m_overrideWindow) {
            subs.window = std::move(m_overrideWindow);
        } else {
            subs.window = std::make_unique<Window>(subs.logger);
            auto windowResult = subs.window->Initialize(config.windowConfig);
            if (!windowResult) {
                subs.logger->Error("Window initialization failed: " + windowResult.error);
                TTF_Quit();
                SDL_Quit();
                return Result<EngineSubsystems>::Failure(windowResult.error);
            }
        }

        // Renderer
        if (m_overrideRenderer) {
            subs.renderer = std::move(m_overrideRenderer);
        } else {
            subs.renderer = std::make_unique<Renderer>(subs.logger);
            auto rendererResult = subs.renderer->Initialize(subs.window.get(), config.rendererConfig);
            if (!rendererResult) {
                subs.logger->Error("Renderer initialization failed: " + rendererResult.error);
                subs.window->Shutdown();
                TTF_Quit();
                SDL_Quit();
                return Result<EngineSubsystems>::Failure(rendererResult.error);
            }
        }

        // Audio engine
        if (m_overrideAudioEngine) {
            subs.audioEngine = std::move(m_overrideAudioEngine);
        } else {
            subs.audioEngine = std::make_unique<AudioEngine>(subs.logger);
            auto audioResult = subs.audioEngine->Initialize();
            if (!audioResult) {
                subs.logger->Warning("Audio engine initialization failed: " + audioResult.error);
                // Continue without audio
            }
        }

        // Input manager
        if (m_overrideInputManager) {
            subs.inputManager = std::move(m_overrideInputManager);
        } else {
            subs.inputManager = std::make_unique<Input::InputManager>(subs.logger);
            subs.inputManager->Initialize();
        }

        // Resource manager
        subs.resourceManager = std::make_unique<Resources::ResourceManager>(
            subs.renderer.get(),
            subs.audioEngine.get(),
            subs.logger
        );
        subs.resourceManager->Initialize();

        // Scene manager
        subs.sceneManager = std::make_unique<SceneManager>(subs.logger);

        subs.logger->Info("Engine subsystems built successfully");
        return Result<EngineSubsystems>::Success(std::move(subs));
    }

} // namespace Engine
