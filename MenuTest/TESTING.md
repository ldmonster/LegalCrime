# Running Tests

## Quick Start

To run the TileMap tests:

```cmd
cd MenuTest\Tests
.\run_tests.bat
```

This will compile and run all unit tests.

## Test Organization

Tests are co-located with the code they test:
- **Engine/World/TileMapTests.cpp** - TileMap component unit tests
- **Engine/World/SimpleTest.h** - Lightweight testing framework

Test runners are centralized in:
- **Tests/run_tests.bat** - Test runner (sets up environment and executes tests)
- **Tests/run_tests_simple.ps1** - PowerShell compilation script

## More Information

See `MenuTest/Tests/README.md` for detailed documentation on:
- Test coverage
- Adding new tests
- Manual compilation
- Available assertions

## Current Test Status

✅ All 34 TileMap tests passing
