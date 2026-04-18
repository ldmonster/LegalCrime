cmake_minimum_required(VERSION 3.20)
project(LegalCrime VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find dependencies via vcpkg or system packages
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_mixer CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

# ============================================================================
# Engine Library
# ============================================================================
set(ENGINE_SOURCES
    Engine/Audio/AudioEngine.cpp
    Engine/Audio/SoundEffect.cpp
    Engine/Camera/Camera2D.cpp
    Engine/Core/Application.cpp
    Engine/Core/EngineBuilder.cpp
    Engine/Core/Logger/Logger.cpp
    Engine/Entity/Entity.cpp
    Engine/Graphics/AnimatedSprite.cpp
    Engine/Graphics/CharacterSprite.cpp
    Engine/Graphics/CharacterSpriteLoader.cpp
    Engine/Graphics/Sprite.cpp
    Engine/Graphics/Texture.cpp
    Engine/Input/InputAction.cpp
    Engine/Input/InputAxis.cpp
    Engine/Input/InputManager.cpp
    Engine/Platform/Window.cpp
    Engine/Renderer/Renderer.cpp
    Engine/Resources/AudioManager.cpp
    Engine/Resources/ResourceManager.cpp
    Engine/Resources/TextureManager.cpp
    Engine/Scene/Scene.cpp
    Engine/Scene/SceneManager.cpp
    Engine/UI/Button.cpp
    Engine/World/Pathfinding.cpp
    Engine/World/TileMap.cpp
    Engine/World/TileMapRenderer.cpp
)

add_library(Engine STATIC ${ENGINE_SOURCES})

target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Engine
)

target_link_libraries(Engine PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer
    SDL3_ttf::SDL3_ttf
)

# ============================================================================
# Game Library
# ============================================================================
set(GAME_SOURCES
    Game/Entities/Character.cpp
    Game/Entities/CharacterFactory.cpp
    Game/LegalCrimeApp.cpp
    Game/Resources/GameResourceLoader.cpp
    Game/Resources/GameResources.cpp
    Game/Scenes/GameplayScene.cpp
    Game/Scenes/MainMenuScene.cpp
    Game/World/Systems/MovementSystem.cpp
    Game/World/Systems/SelectionSystem.cpp
    Game/World/World.cpp
)

add_library(Game STATIC ${GAME_SOURCES})

target_include_directories(Game PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Game
)

target_link_libraries(Game PUBLIC Engine)

# ============================================================================
# Main Executable
# ============================================================================
add_executable(LegalCrime main.cpp)
target_link_libraries(LegalCrime PRIVATE Game)

# ============================================================================
# Tests
# ============================================================================
set(TEST_SOURCES
    Engine/World/TileMapTests.cpp
    Engine/Input/InputActionTests.cpp
    Engine/UI/ButtonTests.cpp
    Engine/Core/TilePositionTests.cpp
    Engine/Core/EventBusTests.cpp
    Engine/World/PathfindingTests.cpp
)

add_executable(LegalCrimeTests ${TEST_SOURCES})

target_include_directories(LegalCrimeTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Engine
    ${CMAKE_CURRENT_SOURCE_DIR}/Tests
)

target_link_libraries(LegalCrimeTests PRIVATE Engine)

# Add a test runner entry point for the test executable
target_sources(LegalCrimeTests PRIVATE Tests/test_main.cpp)
