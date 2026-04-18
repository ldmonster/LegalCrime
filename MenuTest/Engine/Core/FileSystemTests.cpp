#include "../../Tests/SimpleTest.h"
#include "FileSystem.h"
#include <string>

using namespace Engine;
using namespace SimpleTest;

#define PASS return TestResult{__FUNCTION__, true, ""}

// ============================================================================
// FileSystem::JoinPath Tests
// ============================================================================

TEST_CASE(FileSystem_JoinPath_BaseAndRelative) {
    auto result = FileSystem::JoinPath("assets", "textures/hero.png");
    ASSERT_STRING_EQUAL(result, "assets/textures/hero.png");
    PASS;
}

TEST_CASE(FileSystem_JoinPath_BaseWithTrailingSlash) {
    auto result = FileSystem::JoinPath("assets/", "textures/hero.png");
    ASSERT_STRING_EQUAL(result, "assets/textures/hero.png");
    PASS;
}

TEST_CASE(FileSystem_JoinPath_RelativeWithLeadingSlash) {
    auto result = FileSystem::JoinPath("assets", "/textures/hero.png");
    ASSERT_STRING_EQUAL(result, "assets/textures/hero.png");
    PASS;
}

TEST_CASE(FileSystem_JoinPath_BothSlashes) {
    auto result = FileSystem::JoinPath("assets/", "/textures/hero.png");
    ASSERT_STRING_EQUAL(result, "assets/textures/hero.png");
    PASS;
}

TEST_CASE(FileSystem_JoinPath_EmptyBase) {
    auto result = FileSystem::JoinPath("", "textures/hero.png");
    ASSERT_STRING_EQUAL(result, "textures/hero.png");
    PASS;
}

TEST_CASE(FileSystem_JoinPath_EmptyRelative) {
    auto result = FileSystem::JoinPath("assets", "");
    ASSERT_STRING_EQUAL(result, "assets");
    PASS;
}

// ============================================================================
// FileSystem::NormalizePath Tests
// ============================================================================

TEST_CASE(FileSystem_NormalizePath_BackslashesToForward) {
    auto result = FileSystem::NormalizePath("assets\\textures\\hero.png");
    ASSERT_STRING_EQUAL(result, "assets/textures/hero.png");
    PASS;
}

TEST_CASE(FileSystem_NormalizePath_AlreadyNormalized) {
    auto result = FileSystem::NormalizePath("assets/textures/hero.png");
    ASSERT_STRING_EQUAL(result, "assets/textures/hero.png");
    PASS;
}

TEST_CASE(FileSystem_NormalizePath_Mixed) {
    auto result = FileSystem::NormalizePath("assets\\textures/sprites\\hero.png");
    ASSERT_STRING_EQUAL(result, "assets/textures/sprites/hero.png");
    PASS;
}

// ============================================================================
// FileSystem::GetBasePath Tests
// ============================================================================

TEST_CASE(FileSystem_GetBasePath_NotEmpty) {
    const auto& basePath = FileSystem::GetBasePath();
    ASSERT_FALSE(basePath.empty());
    PASS;
}

TEST_CASE(FileSystem_GetBasePath_EndsWithSlash) {
    const auto& basePath = FileSystem::GetBasePath();
    char last = basePath.back();
    ASSERT_TRUE(last == '/' || last == '\\');
    PASS;
}

// ============================================================================
// FileSystem::ResolveAssetPath Tests
// ============================================================================

TEST_CASE(FileSystem_ResolveAssetPath_Prepends_BasePath) {
    auto resolved = FileSystem::ResolveAssetPath("Pics/test.png");
    const auto& base = FileSystem::GetBasePath();
    // Should start with base path
    ASSERT_TRUE(resolved.length() > base.length());
    // Should end with the relative part
    auto suffix = resolved.substr(resolved.length() - std::string("Pics/test.png").length());
    ASSERT_STRING_EQUAL(suffix, "Pics/test.png");
    PASS;
}
