#include "Game/LegalCrimeApp.h"
#include "Engine/Core/Types.h"
#include <cstdio>

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
