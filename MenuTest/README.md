# LegalCrime

An isometric RTS game built on a custom C++17 engine using SDL3.

## Architecture

```
MenuTest/
├── Engine/       # Reusable game-engine library (static)
├── Game/         # Game-specific code (static)
├── Tests/        # Test framework & runner
└── main.cpp      # Entry point
```

**Initialization flow:** `main()` → `LegalCrimeApp` → `Application::Initialize()` → `EngineBuilder` → `Application::Run()`

The engine is built as a static library with interface-based subsystems (IRenderer, IAudioEngine, IWindow). The game layer extends `Application` and registers scenes via `SceneManager`.

## Dependencies

| Library | Purpose |
|---------|---------|
| SDL3 | Windowing, events, core |
| SDL3_image | Image loading |
| SDL3_mixer | Audio playback |
| SDL3_ttf | Font rendering |

Managed via [vcpkg](https://vcpkg.io).

## Building

### Linux (CMake)

```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

### Windows (Visual Studio)

Open `MenuTest.vcxproj` with Visual Studio. vcpkg integration is automatic if installed globally.

## Running Tests

```bash
cmake --build build --target LegalCrimeTests
./build/LegalCrimeTests
```

Tests use a lightweight header-only framework (`Tests/SimpleTest.h`). Test files are co-located with the code they test (e.g., `Engine/World/TileMapTests.cpp`).

## Project Structure

| Directory | Description |
|-----------|-------------|
| `Engine/Audio/` | Sound effects, music player, audio engine |
| `Engine/Camera/` | 2D camera with zoom, follow, shake |
| `Engine/Core/` | Application lifecycle, EventBus, logging, types |
| `Engine/ECS/` | Entity-Component-System (EntityManager, ComponentStorage) |
| `Engine/Entity/` | Entity wrapper & Transform |
| `Engine/Graphics/` | Sprites, animation, character rendering |
| `Engine/Input/` | Keyboard & gamepad input abstraction |
| `Engine/Platform/` | Window abstraction (SDL3) |
| `Engine/Renderer/` | Renderer & SpriteBatch |
| `Engine/Resources/` | Asset management with LRU caching |
| `Engine/Scene/` | Scene stack (push/pop/replace) |
| `Engine/UI/` | UI components (Button, Minimap) |
| `Engine/World/` | TileMap, pathfinding, spatial grid, fog of war |
| `Game/Entities/` | Character definitions & factory |
| `Game/Input/` | Game-specific input bindings |
| `Game/Scenes/` | MainMenu & Gameplay scenes |
| `Game/World/` | Commands, Systems (Movement, Selection, Vision, Steering) |
