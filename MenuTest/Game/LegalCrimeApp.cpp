#include "LegalCrimeApp.h"
#include "Scenes/MainMenuScene.h"
#include "Scenes/GameplayScene.h"
#include "Input/GameInputBindings.h"
#include "Resources/GameResourceLoader.h"
#include "Engine/Core/FileSystem.h"

namespace LegalCrime {

    LegalCrimeApp::LegalCrimeApp()
        : m_gameState(GameState::MainMenu) {
    }

    LegalCrimeApp::~LegalCrimeApp() {
    }
    
    Engine::Result<void> LegalCrimeApp::OnInitialize() {
        GetLogger()->Info("=== Legal Crime Game Initializing ===");

        // Initialize input bindings
        InputBindings::InitializeBindings(GetInputManager());
        GetLogger()->Info("Input bindings initialized");

        // Preload essential resources
        Resources::GameResourceLoader::LoadEssentialResources(GetResourceManager());
        GetLogger()->Info("Essential resources loaded");

        // Log resource statistics
        GetResourceManager()->LogResourceStats();

        // Set window icon (use PNG for cross-platform compatibility)
        if (!GetWindow()->SetIcon(Engine::FileSystem::ResolveAssetPath("Pics/capone.png"))) {
            GetLogger()->Warning("Failed to set window icon");
        }

        // Load music
        if (GetAudioEngine()->IsInitialized()) {
            auto loadResult = GetAudioEngine()->LoadMusicFromDirectory(Engine::FileSystem::ResolveAssetPath("Music/"));
            if (!loadResult) {
                GetLogger()->Warning("Failed to load music directory: " + loadResult.error);
            } else {
                auto playResult = GetAudioEngine()->PlayMusic();
                if (playResult) {
                    GetLogger()->Info("Background music started");
                }
            }
        }
        
        // Load game scenes
        LoadGameScenes();
        
        GetLogger()->Info("Legal Crime game initialized successfully");
        return Engine::Result<void>::Success();
    }
    
    void LegalCrimeApp::OnShutdown() {
        GetLogger()->Info("Legal Crime game shutting down");
    }
    
    void LegalCrimeApp::OnUpdate(float deltaTime) {
        // Handle scene transitions based on menu state
        HandleSceneTransitions();
    }
       
    void LegalCrimeApp::LoadGameScenes() {
        GetLogger()->Info("Loading game scenes...");

        // Create and push main menu scene
        // Note: PushScene will call Initialize() automatically
        auto mainMenuScene = std::make_unique<MainMenuScene>(
            GetLogger(),
            GetRenderer(),
            GetAudioEngine()
        );

        GetSceneManager()->PushScene(std::move(mainMenuScene));
        GetLogger()->Info("Main menu scene loaded");
    }

    void LegalCrimeApp::HandleSceneTransitions() {
        auto currentScene = GetSceneManager()->GetCurrentScene();
        if (!currentScene) {
            return;
        }

        // Check if we're in the main menu
        if (m_gameState == GameState::MainMenu) {
            if (currentScene->GetSceneId() != Engine::SceneId::MainMenu) {
                return;
            }
            auto* menuScene = static_cast<MainMenuScene*>(currentScene);
            // Check if user clicked quit
            if (menuScene->ShouldQuit()) {
                GetLogger()->Info("User requested quit from main menu");
                GetSceneManager()->ClearScenes(); // This will exit the game loop
                return;
            }

            // Check if user clicked single player
            if (menuScene->ShouldStartGame()) {
                GetLogger()->Info("Transitioning from main menu to gameplay");
                m_gameState = GameState::Playing;

                // Create gameplay scene
                // Note: ReplaceScene will call Initialize() automatically
                auto gameplayScene = std::make_unique<GameplayScene>(
                    GetLogger(),
                    GetRenderer(),
                    GetInputManager(),
                    GetResourceManager()
                );

                // Replace menu scene with gameplay scene
                GetSceneManager()->ReplaceScene(std::move(gameplayScene));
                GetLogger()->Info("Transitioned to gameplay scene");
            }
        }
    }
}
