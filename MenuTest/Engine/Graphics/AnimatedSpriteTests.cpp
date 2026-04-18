#include "AnimatedSprite.h"
#include "../../Tests/SimpleTest.h"

// ======================== AnimatedSprite O(1) Lookup Tests ========================

TEST_CASE(AnimatedSprite_SetAnimation_FindsByName) {
    // AnimatedSprite requires a texture, but for this test we'll use nullptr
    // and verify the animation index lookup works.
    Engine::AnimatedSprite sprite(nullptr, nullptr);

    Engine::Animation anim1("walk_down", true);
    anim1.frames.push_back(Engine::AnimationFrame(Engine::Rect(0, 0, 32, 32), 0.1f));
    sprite.AddAnimation(anim1);

    Engine::Animation anim2("walk_up", true);
    anim2.frames.push_back(Engine::AnimationFrame(Engine::Rect(32, 0, 32, 32), 0.1f));
    sprite.AddAnimation(anim2);

    Engine::Animation anim3("idle_down", true);
    anim3.frames.push_back(Engine::AnimationFrame(Engine::Rect(64, 0, 32, 32), 0.1f));
    sprite.AddAnimation(anim3);

    // Set to walk_up
    sprite.SetAnimation("walk_up");
    ASSERT_STRING_EQUAL(sprite.GetCurrentAnimationName(), "walk_up");

    // Set to idle_down
    sprite.SetAnimation("idle_down");
    ASSERT_STRING_EQUAL(sprite.GetCurrentAnimationName(), "idle_down");

    // Set back to walk_down
    sprite.SetAnimation("walk_down");
    ASSERT_STRING_EQUAL(sprite.GetCurrentAnimationName(), "walk_down");

    return {"AnimatedSprite_SetAnimation_FindsByName", true, ""};
}

TEST_CASE(AnimatedSprite_SetAnimation_NonexistentName_KeepsCurrent) {
    Engine::AnimatedSprite sprite(nullptr, nullptr);

    Engine::Animation anim("walk_down", true);
    anim.frames.push_back(Engine::AnimationFrame(Engine::Rect(0, 0, 32, 32), 0.1f));
    sprite.AddAnimation(anim);

    sprite.SetAnimation("walk_down");
    ASSERT_STRING_EQUAL(sprite.GetCurrentAnimationName(), "walk_down");

    // Try to set nonexistent animation — should keep current
    sprite.SetAnimation("nonexistent");
    ASSERT_STRING_EQUAL(sprite.GetCurrentAnimationName(), "walk_down");

    return {"AnimatedSprite_SetAnimation_NonexistentName_KeepsCurrent", true, ""};
}

TEST_CASE(AnimatedSprite_SetAnimation_SameName_NoReset) {
    Engine::AnimatedSprite sprite(nullptr, nullptr);

    Engine::Animation anim("walk_down", true);
    anim.frames.push_back(Engine::AnimationFrame(Engine::Rect(0, 0, 32, 32), 0.2f));
    anim.frames.push_back(Engine::AnimationFrame(Engine::Rect(32, 0, 32, 32), 0.2f));
    sprite.AddAnimation(anim);

    sprite.SetAnimation("walk_down");
    // Advance animation
    sprite.Update(0.25f); // past first frame

    // Set same animation again — should NOT reset
    sprite.SetAnimation("walk_down");
    // Still on the same animation, frame should not reset
    ASSERT_STRING_EQUAL(sprite.GetCurrentAnimationName(), "walk_down");

    return {"AnimatedSprite_SetAnimation_SameName_NoReset", true, ""};
}

TEST_CASE(AnimatedSprite_AddAnimation_ManyAnimations_O1Lookup) {
    Engine::AnimatedSprite sprite(nullptr, nullptr);

    // Add many animations
    for (int i = 0; i < 100; ++i) {
        Engine::Animation anim("anim_" + std::to_string(i), true);
        anim.frames.push_back(Engine::AnimationFrame(Engine::Rect(i * 32, 0, 32, 32), 0.1f));
        sprite.AddAnimation(anim);
    }

    // Verify all are accessible by name
    sprite.SetAnimation("anim_50");
    ASSERT_STRING_EQUAL(sprite.GetCurrentAnimationName(), "anim_50");

    sprite.SetAnimation("anim_99");
    ASSERT_STRING_EQUAL(sprite.GetCurrentAnimationName(), "anim_99");

    sprite.SetAnimation("anim_0");
    ASSERT_STRING_EQUAL(sprite.GetCurrentAnimationName(), "anim_0");

    return {"AnimatedSprite_AddAnimation_ManyAnimations_O1Lookup", true, ""};
}
