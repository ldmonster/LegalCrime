# PowerShell script to add migration files to vcxproj

$projectFile = "MenuTest.vcxproj"
$content = Get-Content $projectFile -Raw

# Find the ClCompile ItemGroup
$compilePattern = '(<ClCompile Include="src\\logger\\Logger.cpp" />)'
$compileReplacement = @'
<ClCompile Include="src\logger\Logger.cpp" />
    <ClCompile Include="Engine\Core\Application.cpp" />
    <ClCompile Include="Engine\Core\Logger\Logger.cpp" />
    <ClCompile Include="Engine\Platform\Window.cpp" />
    <ClCompile Include="Engine\Renderer\Renderer.cpp" />
    <ClCompile Include="Engine\Audio\AudioEngine.cpp" />
    <ClCompile Include="Engine\Scene\Scene.cpp" />
    <ClCompile Include="Engine\Scene\SceneManager.cpp" />
    <ClCompile Include="Migration\WindowBridge.cpp" />
    <ClCompile Include="Migration\RendererBridge.cpp" />
    <ClCompile Include="Migration\MusicBridge.cpp" />
    <ClCompile Include="Migration\GameSectionSceneAdapter.cpp" />
    <ClCompile Include="Game\LegalCrimeApp.cpp" />
'@

$content = $content -replace $compilePattern, $compileReplacement

# Find the ClInclude ItemGroup
$includePattern = '(<ClInclude Include="src\\Menu.hpp" />)'
$includeReplacement = @'
<ClInclude Include="src\Menu.hpp" />
    <ClInclude Include="Engine\Core\Types.h" />
    <ClInclude Include="Engine\Core\Application.h" />
    <ClInclude Include="Engine\Core\Logger\ILogger.h" />
    <ClInclude Include="Engine\Core\Logger\Logger.h" />
    <ClInclude Include="Engine\Platform\IWindow.h" />
    <ClInclude Include="Engine\Platform\Window.h" />
    <ClInclude Include="Engine\Renderer\IRenderer.h" />
    <ClInclude Include="Engine\Renderer\Renderer.h" />
    <ClInclude Include="Engine\Audio\IAudioEngine.h" />
    <ClInclude Include="Engine\Audio\AudioEngine.h" />
    <ClInclude Include="Engine\Scene\IScene.h" />
    <ClInclude Include="Engine\Scene\Scene.h" />
    <ClInclude Include="Engine\Scene\SceneManager.h" />
    <ClInclude Include="Migration\LoggerAdapter.h" />
    <ClInclude Include="Migration\WindowBridge.h" />
    <ClInclude Include="Migration\RendererBridge.h" />
    <ClInclude Include="Migration\MusicBridge.h" />
    <ClInclude Include="Migration\GameSectionSceneAdapter.h" />
    <ClInclude Include="Game\LegalCrimeApp.h" />
'@

$content = $content -replace $includePattern, $includeReplacement

# Save the file
Set-Content $projectFile -Value $content -NoNewline

Write-Host "Project file updated successfully!" -ForegroundColor Green
Write-Host "Added Engine, Migration, and Game files to project." -ForegroundColor Cyan
