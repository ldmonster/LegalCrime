@echo off
REM TileMap Test Runner - Sets up x64 environment and runs tests

echo Setting up x64 build environment...
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1

echo.
echo Running tests...
powershell -ExecutionPolicy Bypass -File "%~dp0run_tests_simple.ps1"

exit /b %ERRORLEVEL%
