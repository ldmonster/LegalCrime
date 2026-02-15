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
    
    Application::Application()
        : m_logger(nullptr)
        , m_initialized(false)
        , m_running(false)
        , m_lastFrameTime(0) {
    }
    
    Application::~Application() {
        Shutdown();
    }
    
    Result<void> Application::Initialize(const ApplicationConfig& config) {
        if (m_initialized) {
            return Result<void>::Failure("Application already initialized");
        }
        
        // Initialize SDL
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
            return Result<void>::Failure(std::string("SDL initialization failed: ") + SDL_GetError());
        }

        // Initialize SDL_ttf
        if (!TTF_Init()) {
            SDL_Quit();
            return Result<void>::Failure(std::string("SDL_ttf initialization failed: ") + SDL_GetError());
        }
        
        // Create logger FIRST - it will be destroyed LAST
        m_logger = new Logger();
        m_logger->SetLogLevel(config.logLevel);
        m_logger->Info("=== Initializing " + config.appName + " ===");

        // Create window
        m_window = std::make_unique<Window>(m_logger);
        auto windowResult = m_window->Initialize(config.windowConfig);
        if (!windowResult) {
            m_logger->Error("Window initialization failed: " + windowResult.error);
            TTF_Quit();
            SDL_Quit();
            return windowResult;
        }
        
        // Create renderer
        m_renderer = std::make_unique<Renderer>(m_logger);
        auto rendererResult = m_renderer->Initialize(m_window.get(), config.rendererConfig);
        if (!rendererResult) {
            m_logger->Error("Renderer initialization failed: " + rendererResult.error);
            m_window->Shutdown();
            TTF_Quit();
            SDL_Quit();
            return rendererResult;
        }
        
        // Create audio engine
        m_audioEngine = std::make_unique<AudioEngine>(m_logger);
        auto audioResult = m_audioEngine->Initialize();
        if (!audioResult) {
            m_logger->Warning("Audio engine initialization failed: " + audioResult.error);
            // Continue without audio
        }

        // Create input manager
        m_inputManager = std::make_unique<Input::InputManager>(m_logger);
        m_inputManager->Initialize();

        // Create resource manager
        m_resourceManager = std::make_unique<Resources::ResourceManager>(
            m_renderer.get(),
            m_audioEngine.get(),
            m_logger
        );
        m_resourceManager->Initialize();

        // Create scene manager
        m_sceneManager = std::make_unique<SceneManager>(m_logger);
        
        m_initialized = true;
        m_logger->Info("Engine initialized successfully");
        
        // Call derived class initialization
        auto gameResult = OnInitialize();
        if (!gameResult) {
            m_logger->Error("Game initialization failed: " + gameResult.error);
            Shutdown();
            return gameResult;
        }
        
        return Result<void>::Success();
    }
    
    void Application::Shutdown() {
        if (!m_initialized) {
            return;
        }

        if (m_logger) {
            m_logger->Info("Shutting down application");
        }

        OnShutdown();

        m_sceneManager.reset();
        m_resourceManager.reset();
        m_inputManager.reset();
        m_audioEngine.reset();
        m_renderer.reset();
        m_window.reset();

        TTF_Quit();
        SDL_Quit();

        if (m_logger) {
            m_logger->Info("Application shutdown complete");
        }

        // Delete logger LAST - after all other subsystems are destroyed
        delete m_logger;
        m_logger = nullptr;
        m_initialized = false;
    }
    
    int Application::Run() {
        if (!m_initialized) {
            return -1;
        }
        
        m_running = true;
        m_lastFrameTime = SDL_GetTicks();
        
        if (m_logger) {
            m_logger->Info("Application started");
        }
        
        while (m_running && m_sceneManager->HasScenes()) {
            ProcessEvents();
            
            float deltaTime = CalculateDeltaTime();
            Update(deltaTime);
            Render();
        }
        
        if (m_logger) {
            m_logger->Info("Application loop ended");
        }
        
        return 0;
    }
    
    void Application::Quit() {
        m_running = false;
    }
    
    void Application::ProcessEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                Quit();
            }

            // Forward event to input manager first
            if (m_inputManager) {
                m_inputManager->ProcessEvent(event);
            }

            // Then to scene manager
            m_sceneManager->HandleEvent(event);
        }
    }

    void Application::Update(float deltaTime) {
        // Update input manager before game logic
        if (m_inputManager) {
            m_inputManager->Update();
        }

        m_sceneManager->Update(deltaTime);
        OnUpdate(deltaTime);
    }
    
    void Application::Render() {
        m_renderer->Clear(Color(0, 0, 0, 255));
        m_sceneManager->Render();
        m_renderer->Present();
    }
    
    float Application::CalculateDeltaTime() {
        uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - m_lastFrameTime) / 1000.0f;
        m_lastFrameTime = currentTime;
        
        // Cap delta time to prevent spiral of death
        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }
        
        return deltaTime;
    }
}
