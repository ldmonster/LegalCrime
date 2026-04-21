#include "Application.h"
#include "EngineBuilder.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#define SDL_IMAGE_USE_SDL3 1
#include <SDL3_image/SDL_image.h>

namespace Engine {
    
    Application::Application()
        : m_logger(nullptr)
        , m_initialized(false)
        , m_running(false)
        , m_lastFrameTime(0)
        , m_fixedAccumulator(0.0f)
        , m_interpolationAlpha(0.0f) {
    }
    
    Application::~Application() {
        Shutdown();
    }
    
    Result<void> Application::Initialize(const ApplicationConfig& config) {
        if (m_initialized) {
            return Result<void>::Failure("Application already initialized");
        }
        
        EngineBuilder builder;
        auto result = builder.Build(config);
        if (!result) {
            return Result<void>::Failure(result.error);
        }

        return Initialize(std::move(result.value));
    }

    Result<void> Application::Initialize(EngineSubsystems subsystems) {
        if (m_initialized) {
            return Result<void>::Failure("Application already initialized");
        }

        m_logger = subsystems.logger;
        m_window = std::move(subsystems.window);
        m_renderer = std::move(subsystems.renderer);
        m_audioEngine = std::move(subsystems.audioEngine);
        m_inputManager = std::move(subsystems.inputManager);
        m_resourceManager = std::move(subsystems.resourceManager);
        m_sceneManager = std::move(subsystems.sceneManager);
        
        m_initialized = true;
        if (m_logger) {
            m_logger->Info("Engine initialized successfully");
        }
        
        // Call derived class initialization
        auto gameResult = OnInitialize();
        if (!gameResult) {
            if (m_logger) {
                m_logger->Error("Game initialization failed: " + gameResult.error);
            }
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
        m_fixedAccumulator = 0.0f;
        m_interpolationAlpha = 0.0f;
        
        if (m_logger) {
            m_logger->Info("Application started");
        }
        
        while (m_running && m_sceneManager->HasScenes()) {
            ProcessEvents();

            float frameTime = CalculateDeltaTime();
            int updateCount = ComputeFixedUpdateCount(
                frameTime,
                FIXED_DT,
                MAX_FRAME_TIME,
                MAX_UPDATES_PER_FRAME,
                m_fixedAccumulator
            );

            for (int i = 0; i < updateCount; ++i) {
                Update(FIXED_DT);
            }

            m_interpolationAlpha = (FIXED_DT > 0.0f) ? (m_fixedAccumulator / FIXED_DT) : 0.0f;
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

        // Reset input frame states after all consumers have read them
        if (m_inputManager) {
            m_inputManager->EndFrame();
        }
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

        return deltaTime;
    }

    int Application::ComputeFixedUpdateCount(
        float frameTime,
        float fixedDt,
        float maxFrameTime,
        int maxUpdatesPerFrame,
        float& accumulator) {

        if (fixedDt <= 0.0f || maxUpdatesPerFrame <= 0) {
            return 0;
        }

        if (frameTime < 0.0f) {
            frameTime = 0.0f;
        }

        if (frameTime > maxFrameTime) {
            frameTime = maxFrameTime;
        }

        accumulator += frameTime;

        int updates = 0;
        while (accumulator >= fixedDt && updates < maxUpdatesPerFrame) {
            accumulator -= fixedDt;
            ++updates;
        }

        return updates;
    }
}
