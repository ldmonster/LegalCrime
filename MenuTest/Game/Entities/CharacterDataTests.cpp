#include "../../Tests/SimpleTest.h"
#include "CharacterData.h"

using namespace LegalCrime::Entities;
using namespace SimpleTest;

// ============================================================================
// CharacterData Default Construction
// ============================================================================

TEST_CASE(CharacterData_DefaultType_IsThug) {
    CharacterData data;
    ASSERT_EQUAL(static_cast<int>(data.type), static_cast<int>(CharacterType::Thug));
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CharacterData_DefaultName_IsUnknown) {
    CharacterData data;
    ASSERT_STRING_EQUAL(data.name, "Unknown");
    return TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// CharacterDataUtils
// ============================================================================

TEST_CASE(CharacterTypeToString_Thug) {
    ASSERT_STRING_EQUAL(CharacterDataUtils::CharacterTypeToString(CharacterType::Thug), "thug");
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CharacterTypeToString_Cop) {
    ASSERT_STRING_EQUAL(CharacterDataUtils::CharacterTypeToString(CharacterType::Cop), "cop");
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CharacterTypeToString_Civilian) {
    ASSERT_STRING_EQUAL(CharacterDataUtils::CharacterTypeToString(CharacterType::Civilian), "civilian");
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(CharacterTypeToString_Unknown) {
    ASSERT_STRING_EQUAL(
        CharacterDataUtils::CharacterTypeToString(static_cast<CharacterType>(99)),
        "unknown"
    );
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(DirectionToString_Down) {
    ASSERT_STRING_EQUAL(CharacterDataUtils::DirectionToString(Direction::Down), "down");
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(GetWalkAnimationForDirection_Down) {
    std::string anim = CharacterDataUtils::GetWalkAnimationForDirection(Direction::Down);
    ASSERT_STRING_EQUAL(anim, "walk_down");
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(GetWalkAnimationForDirection_Up) {
    std::string anim = CharacterDataUtils::GetWalkAnimationForDirection(Direction::Up);
    ASSERT_STRING_EQUAL(anim, "walk_up");
    return TestResult{__FUNCTION__, true, ""};
}
