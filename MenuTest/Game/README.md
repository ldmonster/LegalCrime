# Game

LegalCrime game layer — extends the Engine with RTS-specific systems, scenes, and entities.

## Game States

```
Opening → MainMenu → Playing → Shutdown
```

Managed by `LegalCrimeApp` (extends `Engine::Application`).

## Scenes (`Scenes/`)
- **MainMenuScene** — Background image, sprite-based buttons, sound effects
- **GameplayScene** — Isometric world with ECS systems, camera, fog of war, minimap

## Entities (`Entities/`)
- **Character** — Game unit with CharacterSprite, health, movement
- **CharacterFactory** — Creates characters from `CharacterConfigs.h` definitions
- **CharacterData** — Per-character type configuration

## Input (`Input/`)
- **GameInputBindings** — Maps keyboard/gamepad bindings to game actions (move camera, select, command)

## World Systems (`World/Systems/`)

| System | Purpose |
|--------|---------|
| **CommandSystem** | Per-unit FIFO command queue processing |
| **MovementSystem** | Path following, interpolated movement |
| **SelectionSystem** | Box select, shift-click toggle, ctrl+N control groups |
| **SteeringSystem** | Separation steering / collision avoidance |
| **VisionSystem** | Per-tile fog of war (Unexplored → Explored → Visible) |

## World Commands (`World/Commands/`)

| Command | Description |
|---------|-------------|
| **MoveCommand** | Move to target position with configurable duration |
| **AttackCommand** | Attack target entity within range |

Commands are queued via `CommandSystem`. Shift-click appends; regular click replaces.

## Configuration

- **GameConstants.h** — Named constants for movement, steering, UI layout
- **CharacterConfigs.h** — Character type definitions (sprites, stats)
