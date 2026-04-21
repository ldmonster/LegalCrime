#include "Game/LegalCrimeApp.h"
#include "Engine/Core/ConfigLoader.h"
#include "Engine/Core/FileSystem.h"
#include "Engine/Core/Types.h"
#include <cstdio>
#include <string>

int main(int argc, char* argv[]) {
    // Create game application
    LegalCrime::LegalCrimeApp game;

    // Configure application
    Engine::ApplicationConfig config;
    config.appName = "Legal Crime";
    config.windowConfig.title = "Legal Crime";
    config.windowConfig.resolution = Engine::Resolution(800, 600);
    config.windowConfig.vSync = false;
    config.rendererConfig.vSync = false;
    config.logLevel = Engine::LogLevel::Debug;

    // Optional config override file: --config=/path/to/config.ini
    std::string configPath = Engine::FileSystem::ResolveAssetPath("config.ini");
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        const std::string prefix = "--config=";
        if (arg.rfind(prefix, 0) == 0) {
            configPath = arg.substr(prefix.size());
            break;
        }
    }

    config = Engine::ConfigLoader::LoadOrDefault(configPath, config);
    
    // Initialize
    auto initResult = game.Initialize(config);
    if (!initResult) {
        // Logger may not be available, use stderr
        fprintf(stderr, "Failed to initialize game: %s\n", initResult.error.c_str());
        return -1;
    }
    
    // Run game loop
    int exitCode = game.Run();
    
    // Cleanup happens automatically via RAII
    return exitCode;
}
