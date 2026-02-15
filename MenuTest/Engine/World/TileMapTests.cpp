#include "../../Tests/SimpleTest.h"
#include "TileMap.h"
#include "../Core/Logger/ILogger.h"
#include <iostream>

using namespace Engine;
using namespace SimpleTest;

// Simple null logger for testing
class TestLogger : public ILogger {
public:
    void SetLogLevel(Engine::LogLevel level) override {}
    void Log(Engine::LogLevel level, const std::string& message) override {}
};

// ============================================================================
// Tile Tests
// ============================================================================

TEST_CASE(Tile_DefaultConstructor_InitializesIdToZero) {
    Tile tile;
    ASSERT_EQUAL(tile.GetId(), 0);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Tile_ParametrizedConstructor_SetsIdCorrectly) {
    Tile tile(42);
    ASSERT_EQUAL(tile.GetId(), 42);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Tile_SetId_UpdatesIdValue) {
    Tile tile;
    tile.SetId(100);
    ASSERT_EQUAL(tile.GetId(), 100);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Tile_SetId_AcceptsMaxUint16Value) {
    Tile tile;
    tile.SetId(UINT16_MAX);
    ASSERT_EQUAL(tile.GetId(), UINT16_MAX);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Tile_Render_ReturnsFalseWithNullRenderer) {
    Tile tile;
    Point topLeft(0, 0);
    ASSERT_FALSE(tile.Render(nullptr, topLeft, 30, 15));
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// TileMap Constructor Tests
// ============================================================================

TEST_CASE(TileMap_Constructor_InitializesDimensionsCorrectly) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    ASSERT_EQUAL(tilemap.GetWidth(), 10);
    ASSERT_EQUAL(tilemap.GetHeight(), 10);
    ASSERT_FALSE(tilemap.IsInitialized());
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Constructor_WithoutLogger) {
    TileMap tilemap(5, 5);
    ASSERT_EQUAL(tilemap.GetWidth(), 5);
    ASSERT_EQUAL(tilemap.GetHeight(), 5);
    ASSERT_FALSE(tilemap.IsInitialized());
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Constructor_SetsTileWidthAndHeight) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    ASSERT_EQUAL(tilemap.GetTileWidth(), 30);
    ASSERT_EQUAL(tilemap.GetTileHeight(), 15);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Constructor_InitialOffsetIsZero) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    Point offset = tilemap.GetOffset();
    
    ASSERT_EQUAL(offset.x, 0);
    ASSERT_EQUAL(offset.y, 0);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// TileMap Initialization Tests
// ============================================================================

TEST_CASE(TileMap_Initialize_SucceedsWithValidWindowSize) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    auto result = tilemap.Initialize(1024, 768);
    ASSERT_TRUE(result.success);
    ASSERT_TRUE(tilemap.IsInitialized());
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Initialize_FailsWhenAlreadyInitialized) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    tilemap.Initialize(1024, 768);
    auto result = tilemap.Initialize(1024, 768);
    ASSERT_FALSE(result.success);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Initialize_WithSmallWindowSize) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    auto result = tilemap.Initialize(320, 240);
    ASSERT_TRUE(result.success);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Initialize_WithLargeWindowSize) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    auto result = tilemap.Initialize(4096, 2160);
    ASSERT_TRUE(result.success);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// Tile Access Tests
// ============================================================================

TEST_CASE(TileMap_GetTile_ReturnsValidTileAtOrigin) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    Tile* tile = tilemap.GetTile(0, 0);
    ASSERT_NOT_NULL(tile);
    ASSERT_EQUAL(tile->GetId(), 0);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_GetTile_ReturnsValidTileInMiddle) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    Tile* tile = tilemap.GetTile(5, 5);
    ASSERT_NOT_NULL(tile);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_GetTile_ReturnsValidTileAtMaxBounds) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    Tile* tile = tilemap.GetTile(9, 9);
    ASSERT_NOT_NULL(tile);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_GetTile_ReturnsNullPtrWhenRowOutOfBounds) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    Tile* tile = tilemap.GetTile(10, 0);
    ASSERT_NULL(tile);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_GetTile_ReturnsNullPtrWhenColumnOutOfBounds) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    Tile* tile = tilemap.GetTile(0, 10);
    ASSERT_NULL(tile);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_GetTile_ReturnsNullPtrWhenBothOutOfBounds) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    Tile* tile = tilemap.GetTile(10, 10);
    ASSERT_NULL(tile);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_GetTile_ConstVersionReturnsValidTile) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    const TileMap& constMap = tilemap;
    
    const Tile* tile = constMap.GetTile(0, 0);
    ASSERT_NOT_NULL(tile);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_GetTile_CanModifyTileValues) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    Tile* tile = tilemap.GetTile(0, 0);
    ASSERT_NOT_NULL(tile);
    
    tile->SetId(42);
    ASSERT_EQUAL(tile->GetId(), 42);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_GetTile_AllTilesInitializedToZero) {
    TestLogger logger;
    TileMap tilemap(5, 5, &logger);
    
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            Tile* tile = tilemap.GetTile(i, j);
            if (!tile || tile->GetId() != 0) {
                return SimpleTest::TestResult{__FUNCTION__, false, "Tile not initialized to 0"};
            }
        }
    }
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_GetTile_IndependentTileModification) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    Tile* tile1 = tilemap.GetTile(0, 0);
    Tile* tile2 = tilemap.GetTile(0, 1);
    
    ASSERT_NOT_NULL(tile1);
    ASSERT_NOT_NULL(tile2);
    
    tile1->SetId(10);
    tile2->SetId(20);
    
    ASSERT_EQUAL(tile1->GetId(), 10);
    ASSERT_EQUAL(tile2->GetId(), 20);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// Offset/Camera Tests
// ============================================================================

TEST_CASE(TileMap_SetOffset_UpdatesOffset) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    tilemap.SetOffset(100, 50);
    Point offset = tilemap.GetOffset();
    
    ASSERT_EQUAL(offset.x, 100);
    ASSERT_EQUAL(offset.y, 50);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_SetOffset_WithNegativeValues) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    tilemap.SetOffset(-100, -50);
    Point offset = tilemap.GetOffset();
    
    ASSERT_EQUAL(offset.x, -100);
    ASSERT_EQUAL(offset.y, -50);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_SetOffset_WithLargeValues) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    tilemap.SetOffset(10000, 20000);
    Point offset = tilemap.GetOffset();
    
    ASSERT_EQUAL(offset.x, 10000);
    ASSERT_EQUAL(offset.y, 20000);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_SetOffset_MultipleUpdates) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    tilemap.SetOffset(10, 20);
    ASSERT_EQUAL(tilemap.GetOffset().x, 10);
    
    tilemap.SetOffset(30, 40);
    ASSERT_EQUAL(tilemap.GetOffset().x, 30);
    
    tilemap.SetOffset(-5, -10);
    ASSERT_EQUAL(tilemap.GetOffset().x, -5);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// Property Consistency Tests
// ============================================================================

TEST_CASE(TileMap_PropertyConsistency_DimensionsDoNotChange) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    uint16_t initialWidth = tilemap.GetWidth();
    uint16_t initialHeight = tilemap.GetHeight();
    
    tilemap.Initialize(1024, 768);
    tilemap.SetOffset(100, 100);
    
    ASSERT_EQUAL(tilemap.GetWidth(), initialWidth);
    ASSERT_EQUAL(tilemap.GetHeight(), initialHeight);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_PropertyConsistency_TileSizeConstant) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    uint16_t initialWidth = tilemap.GetTileWidth();
    uint16_t initialHeight = tilemap.GetTileHeight();
    
    tilemap.Initialize(1024, 768);
    
    ASSERT_EQUAL(tilemap.GetTileWidth(), initialWidth);
    ASSERT_EQUAL(tilemap.GetTileHeight(), initialHeight);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_PropertyConsistency_IsometricAspectRatio) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    
    ASSERT_EQUAL(tilemap.GetTileHeight() * 2, tilemap.GetTileWidth());
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// Edge Case Tests
// ============================================================================

TEST_CASE(TileMap_EdgeCase_Minimal1x1Map) {
    TileMap tilemap(1, 1);
    ASSERT_EQUAL(tilemap.GetWidth(), 1);
    ASSERT_EQUAL(tilemap.GetHeight(), 1);
    
    Tile* tile = tilemap.GetTile(0, 0);
    ASSERT_NOT_NULL(tile);
    
    Tile* outOfBounds = tilemap.GetTile(1, 1);
    ASSERT_NULL(outOfBounds);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_EdgeCase_RectangularMapWide) {
    TileMap tilemap(50, 10);
    ASSERT_EQUAL(tilemap.GetWidth(), 50);
    ASSERT_EQUAL(tilemap.GetHeight(), 10);
    
    Tile* corner = tilemap.GetTile(9, 49);
    ASSERT_NOT_NULL(corner);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_EdgeCase_RectangularMapTall) {
    TileMap tilemap(10, 50);
    ASSERT_EQUAL(tilemap.GetWidth(), 10);
    ASSERT_EQUAL(tilemap.GetHeight(), 50);
    
    Tile* corner = tilemap.GetTile(49, 9);
    ASSERT_NOT_NULL(corner);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_EdgeCase_TileIdMaxValue) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);

    Tile* tile = tilemap.GetTile(0, 0);
    ASSERT_NOT_NULL(tile);

    tile->SetId(UINT16_MAX);
    ASSERT_EQUAL(tile->GetId(), UINT16_MAX);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// Arrow Key Event Tests
// ============================================================================

TEST_CASE(TileMap_ArrowKey_UpKeyIncreasesOffsetY) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    Point initialOffset = tilemap.GetOffset();

    SDL_Event event{};
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = SDLK_UP;

    tilemap.HandleEvent(event);

    Point newOffset = tilemap.GetOffset();
    ASSERT_EQUAL(newOffset.x, initialOffset.x);
    ASSERT_EQUAL(newOffset.y, initialOffset.y + 20);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_ArrowKey_DownKeyDecreasesOffsetY) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    Point initialOffset = tilemap.GetOffset();

    SDL_Event event{};
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = SDLK_DOWN;

    tilemap.HandleEvent(event);

    Point newOffset = tilemap.GetOffset();
    ASSERT_EQUAL(newOffset.x, initialOffset.x);
    ASSERT_EQUAL(newOffset.y, initialOffset.y - 20);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_ArrowKey_LeftKeyIncreasesOffsetX) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    Point initialOffset = tilemap.GetOffset();

    SDL_Event event{};
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = SDLK_LEFT;

    tilemap.HandleEvent(event);

    Point newOffset = tilemap.GetOffset();
    ASSERT_EQUAL(newOffset.x, initialOffset.x + 20);
    ASSERT_EQUAL(newOffset.y, initialOffset.y);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_ArrowKey_RightKeyDecreasesOffsetX) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    Point initialOffset = tilemap.GetOffset();

    SDL_Event event{};
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = SDLK_RIGHT;

    tilemap.HandleEvent(event);

    Point newOffset = tilemap.GetOffset();
    ASSERT_EQUAL(newOffset.x, initialOffset.x - 20);
    ASSERT_EQUAL(newOffset.y, initialOffset.y);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_ArrowKey_MultipleUpPresses) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    SDL_Event event{};
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = SDLK_UP;

    tilemap.HandleEvent(event);
    tilemap.HandleEvent(event);
    tilemap.HandleEvent(event);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.y, 60); // 3 * 20
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_ArrowKey_AlternatingDirections) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    SDL_Event upEvent{};
    upEvent.type = SDL_EVENT_KEY_DOWN;
    upEvent.key.key = SDLK_UP;

    SDL_Event downEvent{};
    downEvent.type = SDL_EVENT_KEY_DOWN;
    downEvent.key.key = SDLK_DOWN;

    tilemap.HandleEvent(upEvent);    // +20
    tilemap.HandleEvent(downEvent);  // -20

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.y, 0);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_ArrowKey_CombinedHorizontalVertical) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    SDL_Event upEvent{};
    upEvent.type = SDL_EVENT_KEY_DOWN;
    upEvent.key.key = SDLK_UP;

    SDL_Event leftEvent{};
    leftEvent.type = SDL_EVENT_KEY_DOWN;
    leftEvent.key.key = SDLK_LEFT;

    tilemap.HandleEvent(upEvent);
    tilemap.HandleEvent(leftEvent);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, 20);
    ASSERT_EQUAL(offset.y, 20);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_ArrowKey_WithExistingOffset) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    tilemap.SetOffset(100, 50);

    SDL_Event event{};
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = SDLK_UP;

    tilemap.HandleEvent(event);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, 100);
    ASSERT_EQUAL(offset.y, 70); // 50 + 20
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_ArrowKey_WorksWithNegativeOffset) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    tilemap.SetOffset(-100, -100);

    SDL_Event event{};
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = SDLK_RIGHT;

    tilemap.HandleEvent(event);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, -120); // -100 - 20
    ASSERT_EQUAL(offset.y, -100);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_ArrowKey_IgnoredWhenNotInitialized) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    // Note: NOT calling Initialize

    SDL_Event event{};
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = SDLK_UP;

    tilemap.HandleEvent(event);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, 0);
    ASSERT_EQUAL(offset.y, 0);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_ArrowKey_KeyUpEventDoesNotChangePan) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    SDL_Event event{};
    event.type = SDL_EVENT_KEY_UP;
    event.key.key = SDLK_UP;

    tilemap.HandleEvent(event);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, 0);
    ASSERT_EQUAL(offset.y, 0);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_ArrowKey_NonArrowKeyDoesNotAffectOffset) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    SDL_Event event{};
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = SDLK_SPACE;

    tilemap.HandleEvent(event);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, 0);
    ASSERT_EQUAL(offset.y, 0);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// Bounds Tests
// ============================================================================

TEST_CASE(TileMap_Bounds_GetBoundsMatchesMapSize) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    // Diamond dimensions: (width-1 + height-1) * tileSize
    // For 10x10: (9+9)*30 = 540 width, (9+9)*15 = 270 height
    Rect bounds = tilemap.GetBounds();

    ASSERT_EQUAL(bounds.w, 540);
    ASSERT_EQUAL(bounds.h, 270);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_BoundsCenteredAtWindowCenter) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    Rect bounds = tilemap.GetBounds();

    // Window center is at 512, 384
    // Bounds width is 540, so x should be 512 - 270 = 242
    // Bounds height is 270, so y should be 384 - 135 = 249
    ASSERT_EQUAL(bounds.x, 242);
    ASSERT_EQUAL(bounds.y, 249);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_SetOffsetClampsToPositiveXBound) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    // Diamond half-width = (9+9)*30/2 = 270
    tilemap.SetOffset(500, 0);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, 270); // Clamped to max
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_SetOffsetClampsToNegativeXBound) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    tilemap.SetOffset(-500, 0);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, -270); // Clamped to min
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_SetOffsetClampsToPositiveYBound) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    // Diamond half-height = (9+9)*15/2 = 135
    tilemap.SetOffset(0, 300);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.y, 135); // Clamped to max
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_SetOffsetClampsToNegativeYBound) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    tilemap.SetOffset(0, -300);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.y, -135); // Clamped to min
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_SetOffsetWithinBoundsNotClamped) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    tilemap.SetOffset(50, 30);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, 50);
    ASSERT_EQUAL(offset.y, 30);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_ArrowKeyClampsAtBoundary) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    // Set to near max boundary
    tilemap.SetOffset(260, 0);

    SDL_Event event{};
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = SDLK_LEFT; // Would add 20

    tilemap.HandleEvent(event);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, 270); // Clamped to max (260 + 20 = 280, but max is 270)
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_MultiplePressesStopAtBoundary) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    SDL_Event event{};
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = SDLK_UP;

    // Press up 10 times (would be 200 without bounds)
    for (int i = 0; i < 10; i++) {
        tilemap.HandleEvent(event);
    }

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.y, 135); // Clamped to max Y
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_DifferentMapSizeHasDifferentBounds) {
    TestLogger logger;
    TileMap tilemap(20, 20, &logger); // Larger map
    tilemap.Initialize(1024, 768);

    // Diamond: (19+19)*30 = 1140, (19+19)*15 = 570
    Rect bounds = tilemap.GetBounds();

    ASSERT_EQUAL(bounds.w, 1140);
    ASSERT_EQUAL(bounds.h, 570);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_LargerMapAllowsMorePanning) {
    TestLogger logger;
    TileMap tilemap(20, 20, &logger);
    tilemap.Initialize(1024, 768);

    // Diamond half-width = (19+19)*30/2 = 570
    tilemap.SetOffset(800, 0);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, 570); // Larger bound than 10x10 map
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_BothAxesClamped) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    tilemap.SetOffset(500, 500);

    Point offset = tilemap.GetOffset();
    // With diamond bounds: |x|/270 + |y|/135 <= 1
    // 500/270 + 500/135 = 1.852 + 3.704 = 5.556
    // Scale factor = 1/5.556 = 0.18
    // Result: x=90, y=90
    ASSERT_EQUAL(offset.x, 90);
    ASSERT_EQUAL(offset.y, 90);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_NegativeBothAxesClamped) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    tilemap.SetOffset(-500, -500);

    Point offset = tilemap.GetOffset();
    // Same scaling as positive: scale = 0.18
    // Result: x=-90, y=-90
    ASSERT_EQUAL(offset.x, -90);
    ASSERT_EQUAL(offset.y, -90);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_EdgeCaseZeroOffset) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    tilemap.SetOffset(0, 0);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, 0);
    ASSERT_EQUAL(offset.y, 0);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_ExactlyAtPositiveBoundary) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    // Diamond vertices: (±270, 0) and (0, ±135)
    tilemap.SetOffset(270, 0);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, 270);
    ASSERT_EQUAL(offset.y, 0);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_ExactlyAtNegativeBoundary) {
    TestLogger logger;
    TileMap tilemap(10, 10, &logger);
    tilemap.Initialize(1024, 768);

    tilemap.SetOffset(-270, 0);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, -270);
    ASSERT_EQUAL(offset.y, 0);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// Non-Square Map Bounds Tests
// ============================================================================

TEST_CASE(TileMap_Bounds_WideMap_HorizontalBounds) {
    TestLogger logger;
    TileMap tilemap(100, 10, &logger); // 100 wide, 10 tall
    tilemap.Initialize(1024, 768);

    // Diamond width: (99+9)*30 = 3240, half = 1620
    tilemap.SetOffset(2000, 0);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, 1620); // Clamped to max
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_WideMap_VerticalBounds) {
    TestLogger logger;
    TileMap tilemap(100, 10, &logger); // 100 wide, 10 tall
    tilemap.Initialize(1024, 768);

    // Diamond height: (99+9)*15 = 1620, half = 810
    tilemap.SetOffset(0, 1000);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.y, 810); // Clamped to max
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_TallMap_HorizontalBounds) {
    TestLogger logger;
    TileMap tilemap(10, 100, &logger); // 10 wide, 100 tall
    tilemap.Initialize(1024, 768);

    // Diamond width: (9+99)*30 = 3240, half = 1620
    // SAME as 100×10 because diamond formula is symmetric!
    tilemap.SetOffset(2000, 0);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.x, 1620); // Clamped to max
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_TallMap_VerticalBounds) {
    TestLogger logger;
    TileMap tilemap(10, 100, &logger); // 10 wide, 100 tall
    tilemap.Initialize(1024, 768);

    // Diamond height: (9+99)*15 = 1620, half = 810
    // SAME as 100×10!
    tilemap.SetOffset(0, 1000);

    Point offset = tilemap.GetOffset();
    ASSERT_EQUAL(offset.y, 810); // Clamped to max
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_TallMap_DiagonalClamping) {
    TestLogger logger;
    TileMap tilemap(10, 100, &logger);
    tilemap.Initialize(1024, 768);

    // halfWidth = 1620, halfHeight = 810
    // Try to set (1620, 810) which is on the diamond edge
    tilemap.SetOffset(1620, 810);

    Point offset = tilemap.GetOffset();
    // Distance = 1620/1620 + 810/810 = 1 + 1 = 2.0
    // Scale = 1/2.0 = 0.5
    // Result: (810, 405)
    ASSERT_EQUAL(offset.x, 810);
    ASSERT_EQUAL(offset.y, 405);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_WideMap_GetBounds) {
    TestLogger logger;
    TileMap tilemap(100, 10, &logger);
    tilemap.Initialize(1024, 768);

    Rect bounds = tilemap.GetBounds();

    // Diamond: (99+9)*30 = 3240, (99+9)*15 = 1620
    ASSERT_EQUAL(bounds.w, 3240);
    ASSERT_EQUAL(bounds.h, 1620);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(TileMap_Bounds_TallMap_GetBounds) {
    TestLogger logger;
    TileMap tilemap(10, 100, &logger);
    tilemap.Initialize(1024, 768);

    Rect bounds = tilemap.GetBounds();

    // Diamond: (9+99)*30 = 3240, (9+99)*15 = 1620
    // SAME as 100×10!
    ASSERT_EQUAL(bounds.w, 3240);
    ASSERT_EQUAL(bounds.h, 1620);
    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}

int main() {
    return SimpleTest::TestRunner::RunAll();
}

