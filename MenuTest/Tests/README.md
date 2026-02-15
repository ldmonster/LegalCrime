# TileMap Tests

This directory contains test infrastructure for the TileMap component.

## Files

- **SimpleTest.h** - Lightweight test framework (header-only)
- **run_tests.bat** - Batch file to set up environment and run tests
- **run_tests_simple.ps1** - PowerShell script that compiles and runs tests
- **Output/** - Compiled test executable and dependencies (auto-generated)

## Test Source Files

The actual test implementations are co-located with the code they test:
- **`../Engine/World/TileMapTests.cpp`** - Unit tests for Tile and TileMap classes

## Running the Tests

### Method 1: Using Batch File (Easiest)

Simply double-click or run from command prompt:

```cmd
cd MenuTest\Tests
.\run_tests.bat
```

This will automatically:
- Set up the x64 Visual Studio build environment
- Compile the tests
- Copy required DLLs
- Run all tests

### Method 2: Manual Compilation

From an **x64 Native Tools Command Prompt for VS**:

```cmd
cd MenuTest\Tests
.\run_tests_simple.ps1
```

## Test Coverage

The test suite covers:

### Tile Class
- Default and parameterized constructors
- ID getters and setters
- Max value handling (UINT16_MAX)
- Rendering with null renderer

### TileMap Class
- **Constructor Tests**: Various configurations and dimensions
- **Initialization Tests**: Window size validation
- **Tile Access Tests**: Boundary checking, const correctness
- **Offset/Camera Tests**: Position manipulation
- **Property Tests**: Dimension and tile size consistency
- **Edge Cases**: 1x1 maps, rectangular maps, max values

## Test Results

All tests should pass with a green `[PASS]` indicator. Any failures will show:
- Test name
- Error message with expected vs actual values

Example output:
```
=== TileMap Test Suite ===

[PASS] Tile_DefaultConstructor_InitializesIdToZero_impl
[PASS] TileMap_Constructor_InitializesDimensionsCorrectly_impl
...

=== Test Summary ===
Passed: 34
Failed: 0
Total:  34
```

## Adding New Tests

To add a new test, open `TileMapTests.cpp` and use the `TEST_CASE` macro:

```cpp
TEST_CASE(YourTestName) {
    // Arrange
    TileMap tilemap(10, 10);

    // Act
    auto result = tilemap.SomeOperation();

    // Assert
    ASSERT_TRUE(result);
    ASSERT_EQUAL(value, expected);

    return SimpleTest::TestResult{__FUNCTION__, true, ""};
}
```

Available assertions:
- `ASSERT_EQUAL(actual, expected)`
- `ASSERT_TRUE(condition)`
- `ASSERT_FALSE(condition)`
- `ASSERT_NOT_NULL(ptr)`
- `ASSERT_NULL(ptr)`

## Notes

- These tests are **NOT** included in the main application build
- Tests are compiled and run independently
- The test framework is lightweight and requires no external test dependencies
- SDL3 libraries are automatically copied to the Output directory at runtime
