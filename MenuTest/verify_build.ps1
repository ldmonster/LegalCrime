# Build Verification Script
# Checks for common issues before building

Write-Host "`n=== Build Verification ===" -ForegroundColor Cyan

# Check if all required files exist
$requiredFiles = @(
    "Engine\Core\Application.cpp",
    "Engine\Core\Logger\Logger.cpp",
    "Engine\Platform\Window.cpp",
    "Engine\Renderer\Renderer.cpp",
    "Engine\Audio\AudioEngine.cpp",
    "Engine\Scene\Scene.cpp",
    "Engine\Scene\SceneManager.cpp",
    "Migration\WindowBridge.cpp",
    "Migration\RendererBridge.cpp",
    "Migration\MusicBridge.cpp",
    "Migration\GameSectionSceneAdapter.cpp",
    "Game\LegalCrimeApp.cpp",
    "main_new.cpp"
)

$missing = @()
foreach ($file in $requiredFiles) {
    if (!(Test-Path $file)) {
        $missing += $file
    }
}

if ($missing.Count -gt 0) {
    Write-Host "`n❌ Missing files:" -ForegroundColor Red
    $missing | ForEach-Object { Write-Host "  - $_" -ForegroundColor Yellow }
    exit 1
}

Write-Host "✅ All required files present" -ForegroundColor Green

# Check project file
$projContent = Get-Content MenuTest.vcxproj -Raw

$requiredIncludes = @(
    "Engine\Core\Application.cpp",
    "Migration\WindowBridge.cpp",
    "Game\LegalCrimeApp.cpp"
)

$notInProject = @()
foreach ($include in $requiredIncludes) {
    if ($projContent -notmatch [regex]::Escape($include)) {
        $notInProject += $include
    }
}

if ($notInProject.Count -gt 0) {
    Write-Host "`n❌ Files not in project:" -ForegroundColor Red
    $notInProject | ForEach-Object { Write-Host "  - $_" -ForegroundColor Yellow }
    exit 1
}

Write-Host "✅ All files are in project" -ForegroundColor Green

# Check for duplicate main
$mainCount = 0
if ($projContent -match '<ClCompile Include="main.cpp"[^>]*>' -and $projContent -notmatch '<ClCompile Include="main.cpp"><ExcludedFromBuild>true</ExcludedFromBuild>') {
    $mainCount++
}
if ($projContent -match '<ClCompile Include="main_new.cpp"') {
    $mainCount++
}

if ($mainCount -gt 1) {
    Write-Host "`n⚠️  Warning: Multiple main() functions detected" -ForegroundColor Yellow
    Write-Host "   main.cpp should be excluded from build" -ForegroundColor Yellow
} else {
    Write-Host "✅ No duplicate main() functions" -ForegroundColor Green
}

Write-Host "`n=== Ready to Build! ===" -ForegroundColor Cyan
Write-Host "In Visual Studio:" -ForegroundColor White
Write-Host "  1. Close and reopen solution (to refresh)" -ForegroundColor Gray
Write-Host "  2. Build > Rebuild Solution (Ctrl+Shift+B)" -ForegroundColor Gray
Write-Host "  3. Check Output window for any errors" -ForegroundColor Gray
Write-Host ""
