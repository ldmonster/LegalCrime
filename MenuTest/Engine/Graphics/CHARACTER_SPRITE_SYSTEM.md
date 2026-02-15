# Character Sprite System

This document describes the character sprite loading system designed for managing multiple characters with different sprite sheets in an RTS game.

## Overview

The character sprite system consists of three main components:

1. **CharacterSpriteConfig.h** - Configuration structures for defining sprite layouts
2. **CharacterSprite** - Wrapper class that manages a character's sprite and animations
3. **CharacterSpriteLoader** - Factory class for creating characters from configurations

## Components

### 1. CharacterSpriteConfig

Defines the configuration for a character's sprite sheet and animations.

```cpp
struct AnimationConfig {
    std::string name;           // Animation name (e.g., "walk_left", "idle")
    uint16_t frameWidth;        // Width of each frame in pixels
    uint16_t frameHeight;       // Height of each frame in pixels
    uint16_t sheetColumns;      // Total columns in the sprite sheet
    uint16_t sheetRows;         // Total rows in the sprite sheet
    uint16_t startFrame;        // Starting frame index (0-based)
    uint16_t endFrame;          // Ending frame index (inclusive)
    float frameDuration;        // Duration of each frame in seconds
    bool loop;                  // Whether the animation should loop
};

struct CharacterSpriteConfig {
    std::string characterType;          // Character type identifier
    std::string spriteSheetPath;        // Path to the sprite sheet texture
    float defaultScale;                 // Default scale factor for the sprite
    std::vector<AnimationConfig> animations;  // All animations for this character
    std::string defaultAnimation;       // Name of the default/idle animation
};
```

### 2. CharacterSprite

High-level interface for managing a character's sprite with animations.

**Key Methods:**
- `AddAnimation(const AnimationConfig& config)` - Add an animation from config
- `SetAnimation(const std::string& name)` - Switch to a different animation
- `Update(float deltaTime)` - Update animation (call each frame)
- `Render(IRenderer* renderer, int x, int y)` - Render the character
- `SetPosition(int x, int y)` / `GetPosition(int& x, int& y)` - Position management
- `SetScale(float scale)` / `GetScale()` - Scale management

### 3. CharacterSpriteLoader

Factory for creating character sprites with texture caching.

**Key Methods:**
- `RegisterConfig(const CharacterSpriteConfig& config)` - Register a character type
- `CreateCharacter(const std::string& characterType)` - Create a character instance
- `HasConfig(const std::string& characterType)` - Check if a type is registered
- `ClearConfigs()` - Clear all registered configurations

**Features:**
- Texture caching: Shares textures between multiple characters of the same type
- Configuration management: Pre-register character types for easy creation
- Error handling: Returns nullptr if character creation fails

## Usage Example

### Step 1: Define Character Configurations

Create configurations in `CharacterConfigs.h`:

```cpp
Engine::CharacterSpriteConfig CreateThugConfig() {
    Engine::CharacterSpriteConfig config;
    config.characterType = "thug";
    config.spriteSheetPath = "Pics/Thug_Frames.png";
    config.defaultScale = 2.0f;
    config.defaultAnimation = "idle";

    // Add idle animation
    config.animations.push_back(Engine::AnimationConfig(
        "idle",      // name
        50,          // frame width
        50,          // frame height
        6,           // columns
        4,           // rows
        0,           // start frame
        2,           // end frame
        0.2f,        // frame duration
        true         // loop
    ));

    // Add walk animations
    config.animations.push_back(Engine::AnimationConfig(
        "walk_left", 50, 50, 6, 4, 19, 24, 0.1f, true
    ));

    // ... add more animations

    return config;
}
```

### Step 2: Initialize the Loader

```cpp
// Create the sprite loader
m_spriteLoader = std::make_unique<Engine::CharacterSpriteLoader>(
    m_renderer, 
    m_logger
);

// Register character configurations
m_spriteLoader->RegisterConfig(CharacterConfigs::CreateThugConfig());
m_spriteLoader->RegisterConfig(CharacterConfigs::CreateCopConfig());
m_spriteLoader->RegisterConfig(CharacterConfigs::CreateCivilianConfig());
```

### Step 3: Create Characters

```cpp
// Create a thug character
auto thug = m_spriteLoader->CreateCharacter("thug");

// Create a cop character
auto cop = m_spriteLoader->CreateCharacter("cop");

// Create multiple civilians (shares the same texture)
auto civilian1 = m_spriteLoader->CreateCharacter("civilian");
auto civilian2 = m_spriteLoader->CreateCharacter("civilian");
```

### Step 4: Use Characters in Game Loop

```cpp
// Update (call every frame)
void Update(float deltaTime) {
    thug->Update(deltaTime);
    cop->Update(deltaTime);
}

// Render
void Render() {
    int x, y;
    thug->GetPosition(x, y);
    thug->Render(m_renderer, x, y);
    
    cop->GetPosition(x, y);
    cop->Render(m_renderer, x, y);
}

// Change animation based on game logic
void HandleMovement() {
    if (movingLeft) {
        thug->SetAnimation("walk_left");
    } else if (movingRight) {
        thug->SetAnimation("walk_right");
    } else {
        thug->SetAnimation("idle");
    }
}
```

## Adding New Character Types

To add a new character type:

1. **Create the sprite sheet** - Ensure your sprite sheet is in a grid layout

2. **Define the configuration** in `CharacterConfigs.h`:
   ```cpp
   static Engine::CharacterSpriteConfig CreateNewCharacterConfig() {
       Engine::CharacterSpriteConfig config;
       config.characterType = "new_character";
       config.spriteSheetPath = "Pics/NewCharacter.png";
       config.defaultScale = 1.5f;
       config.defaultAnimation = "idle";
       
       // Add all animations for this character
       config.animations.push_back(...);
       
       return config;
   }
   ```

3. **Register the configuration** during initialization:
   ```cpp
   m_spriteLoader->RegisterConfig(CharacterConfigs::CreateNewCharacterConfig());
   ```

4. **Create instances** as needed:
   ```cpp
   auto character = m_spriteLoader->CreateCharacter("new_character");
   ```

## Benefits for RTS Games

1. **Memory Efficient**: Texture caching ensures multiple units of the same type share one texture
2. **Flexible**: Easy to support different sprite sheet layouts (different frame sizes, counts, etc.)
3. **Organized**: All character definitions in one place (`CharacterConfigs.h`)
4. **Reusable**: Create as many instances as needed with simple function calls
5. **Maintainable**: Changes to a character type only require updating its configuration

## Migration from Old System

The old system loaded sprites directly in `GameplayScene::Initialize()`:

```cpp
// OLD WAY (hardcoded in scene)
auto texture = Texture::LoadFromFile(...);
m_character = std::make_unique<AnimatedSprite>(texture, ...);
m_character->AddAnimation(...);
m_character->SetAnimation("idle");
```

New system using configuration:

```cpp
// NEW WAY (configuration-based)
m_spriteLoader->RegisterConfig(CharacterConfigs::CreateThugConfig());
m_character = m_spriteLoader->CreateCharacter("thug");
// Animations are already set up from config!
```

## Performance Considerations

- **Texture Caching**: The loader caches textures, so creating 100 thugs only loads the texture once
- **Shared Ownership**: Textures use `std::shared_ptr`, automatically cleaned up when no longer needed
- **Animation Data**: Animation configurations are copied per instance (small memory footprint)

## Future Enhancements

Potential improvements:

1. **JSON/XML Configuration**: Load character configs from files instead of code
2. **Animation Events**: Trigger events at specific frames (e.g., footstep sounds)
3. **Sprite Atlases**: Support multiple characters in a single texture atlas
4. **Hot Reloading**: Reload character configurations without restarting the game
