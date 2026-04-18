# Engine

Reusable C++17 game-engine library built on SDL3. Provides interface-based subsystems assembled via `EngineBuilder`.

## Subsystems

### Core (`Core/`)
- **Application** — Main loop, subsystem ownership, virtual hooks (`OnInitialize`, `OnUpdate`, `OnShutdown`)
- **EngineBuilder** — Constructs and wires all subsystems
- **EventBus** — Thread-safe publish/subscribe event system
- **Types.h** — `Result<T>` error handling, `Rect`, `Vector2`, `DisplayScale`
- **Constants.h** — Named engine defaults (camera, pathfinding, window)
- **FileSystem** — Cross-platform path utilities (SDL_GetBasePath-based)
- **Logger** — Leveled logging with thread-local timestamp buffers

### Audio (`Audio/`)
- **IAudioEngine** — Interface for music and sound playback
- **AudioEngine** — SDL3_mixer implementation, delegates music to `MusicPlayer`
- **MusicPlayer** — Streaming playlist with shuffle, looping, mutex-protected state
- **SoundEffect** — Individual sound effect wrapper

### Graphics (`Graphics/`)
- **Sprite** — Static textured quad with source rect
- **AnimatedSprite** — Frame-based animation with SmoothMovement
- **CharacterSprite** — Multi-direction character animation (walk, idle, attack)
- **Texture** — SDL_Texture RAII wrapper with `LoadFromFile`

### Input (`Input/`)
- **InputManager** — Polls keyboard & gamepad, manages actions/axes
- **InputAction** — Digital button binding (keyboard + gamepad)
- **InputAxis** — Analog axis binding with dead zones

### Renderer (`Renderer/`)
- **IRenderer** — Rendering interface
- **Renderer** — SDL3 renderer implementation
- **SpriteBatch** — Texture-sorted batched drawing

### Resources (`Resources/`)
- **ResourceManager** — Central asset registry
- **TextureManager** — Texture cache with LRU eviction and pinning
- **AudioManager** — Music/SFX registration

### Scene (`Scene/`)
- **IScene** — Scene lifecycle interface (Initialize, Update, Render, Shutdown)
- **SceneManager** — Stack-based scene management (Push, Pop, Replace)

### ECS (`ECS/`)
- **EntityManager** — Entity lifecycle with ID recycling (free-list)
- **ComponentStorage** — Dense-array storage with sparse lookup
- **System** — Base class for ECS systems

### World (`World/`)
- **TileMap** — Isometric tile grid
- **TileMapRenderer** — Chunk-cached viewport rendering
- **Pathfinding** — A* with object-pooled nodes, path smoothing
- **SpatialGrid** — Fixed-cell spatial partitioning
- **FogOfWarRenderer** — Isometric fog overlay

### Platform (`Platform/`)
- **IWindow** — Window interface with high-DPI support
- **Window** — SDL3 window implementation

### UI (`UI/`)
- **Button** — Sprite-based button with hover/press/click callbacks
- **Minimap** — Downscaled world view with camera viewport indicator

## Error Handling

All fallible operations return `Result<T>` (defined in `Core/Types.h`). No exceptions in engine code.

```cpp
auto result = texture->LoadFromFile(renderer, path, logger);
if (!result) {
    logger->Error(result.error);
}
```

## Threading Model

Game logic is single-threaded. Only audio callbacks and EventBus may be accessed from multiple threads. EventBus uses `std::shared_mutex`; MusicPlayer uses `std::mutex`.
