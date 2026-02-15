# Simple TileMap Test Runner
# Run this from an x64 Native Tools Command Prompt for VS

Write-Host "=== TileMap Test Runner ===" -ForegroundColor Cyan
Write-Host "Checking environment..." -ForegroundColor Yellow

# Check if we're in the correct build environment
$clPath = (Get-Command cl.exe -ErrorAction SilentlyContinue).Source
if (-not $clPath) {
    Write-Host "Error: cl.exe not found!" -ForegroundColor Red
    Write-Host "Please run from 'x64 Native Tools Command Prompt for VS 2024'" -ForegroundColor Yellow
    exit 1
}

if ($clPath -notmatch "x64") {
    Write-Host "Warning: Using 32-bit compiler, but need 64-bit" -ForegroundColor Yellow
    Write-Host "Current compiler: $clPath" -ForegroundColor Gray
    Write-Host ""
    Write-Host "Please run from 'x64 Native Tools Command Prompt for VS 2024'" -ForegroundColor Yellow
    Write-Host "Or run VsDevCmd.bat with -arch=x64 parameter" -ForegroundColor Yellow
    exit 1
}

Write-Host "Compiler: $clPath" -ForegroundColor Green
Write-Host ""

# Paths
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$testDir = Split-Path -Parent $scriptDir
$testFile = "$testDir\Engine\World\TileMapTests.cpp"
$outDir = Join-Path $scriptDir "Output"
$outExe = Join-Path $outDir "TileMapTests.exe"

# Create output directory
if (-not (Test-Path $outDir)) {
    New-Item -ItemType Directory -Path $outDir | Out-Null
}

# Source files
$sources = @(
    $testFile,
    "$testDir\Engine\World\TileMap.cpp",
    "$testDir\Engine\Core\Logger\Logger.cpp",
    "$testDir\Engine\Graphics\Texture.cpp"
)

# Include paths
$includes = @(
    "/I$testDir",
    "/IC:\vslib\SDL3-3.4.0\include",
    "/IC:\vslib\SDL3_image-3.4.0\include",
    "/IC:\vslib\SDL3_ttf-3.2.2\include"
)

# Library paths
$libPaths = @(
    "/LIBPATH:C:\vslib\SDL3-3.4.0\lib\x64",
    "/LIBPATH:C:\vslib\SDL3_image-3.4.0\lib\x64",
    "/LIBPATH:C:\vslib\SDL3_ttf-3.2.2\lib\x64"
)

# Libraries
$libs = @("SDL3.lib", "SDL3_image.lib", "SDL3_ttf.lib")

Write-Host "Compiling tests..." -ForegroundColor Yellow

# Build command
$compileArgs = @("/EHsc", "/std:c++17", "/MD", "/Fe:$outExe") + $includes + $sources + @("/link") + $libPaths + $libs

# Compile
Write-Host "Running: cl.exe..." -ForegroundColor Gray
$output = & cl.exe $compileArgs 2>&1

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "Compilation FAILED!" -ForegroundColor Red
    Write-Host $output
    exit 1
}

Write-Host "Compilation successful!" -ForegroundColor Green
Write-Host ""

# Run tests
Write-Host "Running tests..." -ForegroundColor Cyan
Write-Host ("=" * 60) -ForegroundColor Gray

# Copy required DLLs to output directory
$dllsources = @(
    "C:\vslib\SDL3-3.4.0\lib\x64\SDL3.dll",
    "C:\vslib\SDL3_image-3.4.0\lib\x64\SDL3_image.dll",
    "C:\vslib\SDL3_ttf-3.2.2\lib\x64\SDL3_ttf.dll"
)

foreach ($dll in $dllsources) {
    if (Test-Path $dll) {
        $dllName = Split-Path $dll -Leaf
        Copy-Item $dll $outDir -Force -ErrorAction SilentlyContinue
    }
}

& $outExe
$exitCode = $LASTEXITCODE
Write-Host ("=" * 60) -ForegroundColor Gray
Write-Host ""

if ($exitCode -eq 0) {
    Write-Host "All tests PASSED!" -ForegroundColor Green
} else {
    Write-Host "Some tests FAILED (exit code: $exitCode)" -ForegroundColor Red
}

exit $exitCode
