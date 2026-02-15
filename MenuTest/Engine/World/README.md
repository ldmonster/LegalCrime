# TileMap Module

This directory contains the TileMap implementation and its tests.

## Files

### Implementation
- **TileMap.h** - TileMap and Tile class declarations
- **TileMap.cpp** - TileMap and Tile class implementations

### Tests
- **TileMapTests.cpp** - Unit tests (34 tests covering all TileMap functionality)
  - Uses `../../Tests/SimpleTest.h` for the test framework

## Running Tests

To run the tests:

```cmd
cd MenuTest\Tests
.\run_tests.bat
```

Or see `MenuTest/TESTING.md` for more options.

## Test Coverage

The test suite covers:
- Tile construction, ID management, rendering
- TileMap construction with various dimensions
- Initialization and window size handling
- Tile access with boundary validation
- Camera offset manipulation
- Property consistency
- Edge cases (1x1 maps, rectangular maps, max values)

All 34 tests currently passing ✅
