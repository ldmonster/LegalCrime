#include "../../Tests/SimpleTest.h"
#include "Entity.h"
#include "Transform.h"
#include <cmath>

using namespace Engine;
using namespace SimpleTest;

// ============================================================================
// Transform Tests
// ============================================================================

TEST_CASE(Transform_DefaultValues) {
    Transform t;
    ASSERT_EQUAL(t.position.x, 0);
    ASSERT_EQUAL(t.position.y, 0);
    ASSERT_FLOAT_NEAR(t.rotation, 0.0f, 0.001f);
    ASSERT_FLOAT_NEAR(t.scale, 1.0f, 0.001f);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Transform_ConstructWithPosition) {
    Transform t(100, 200);
    ASSERT_EQUAL(t.position.x, 100);
    ASSERT_EQUAL(t.position.y, 200);
    ASSERT_FLOAT_NEAR(t.rotation, 0.0f, 0.001f);
    ASSERT_FLOAT_NEAR(t.scale, 1.0f, 0.001f);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Transform_ConstructFull) {
    Transform t(10, 20, 45.0f, 2.0f);
    ASSERT_EQUAL(t.position.x, 10);
    ASSERT_EQUAL(t.position.y, 20);
    ASSERT_FLOAT_NEAR(t.rotation, 45.0f, 0.001f);
    ASSERT_FLOAT_NEAR(t.scale, 2.0f, 0.001f);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Transform_SetPosition) {
    Transform t;
    t.SetPosition(42, 84);
    ASSERT_EQUAL(t.position.x, 42);
    ASSERT_EQUAL(t.position.y, 84);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Transform_SetPositionPoint) {
    Transform t;
    Point p{15, 30};
    t.SetPosition(p);
    ASSERT_EQUAL(t.position.x, 15);
    ASSERT_EQUAL(t.position.y, 30);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Transform_Translate) {
    Transform t(10, 20);
    t.Translate(5, -3);
    ASSERT_EQUAL(t.position.x, 15);
    ASSERT_EQUAL(t.position.y, 17);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Transform_Rotate_Normalize) {
    Transform t;
    t.Rotate(370.0f);
    ASSERT_FLOAT_NEAR(t.rotation, 10.0f, 0.001f);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Transform_Rotate_NegativeNormalize) {
    Transform t;
    t.SetRotation(-10.0f);
    ASSERT_FLOAT_NEAR(t.rotation, 350.0f, 0.001f);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Transform_Scale) {
    Transform t;
    t.SetScale(2.5f);
    ASSERT_FLOAT_NEAR(t.scale, 2.5f, 0.001f);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Transform_ScaleBy) {
    Transform t;
    t.SetScale(2.0f);
    t.ScaleBy(1.5f);
    ASSERT_FLOAT_NEAR(t.scale, 3.0f, 0.001f);
    return TestResult{__FUNCTION__, true, ""};
}

// ============================================================================
// Entity Tests
// ============================================================================

TEST_CASE(Entity_UniqueIds) {
    Entity e1("a");
    Entity e2("b");
    Entity e3("c");

    ASSERT_TRUE(e1.GetId() != e2.GetId());
    ASSERT_TRUE(e2.GetId() != e3.GetId());
    ASSERT_TRUE(e1.GetId() != e3.GetId());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Entity_NameStored) {
    Entity e("TestEntity");
    ASSERT_STRING_EQUAL(e.GetName(), "TestEntity");
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Entity_SetName) {
    Entity e("Old");
    e.SetName("New");
    ASSERT_STRING_EQUAL(e.GetName(), "New");
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Entity_DefaultActive) {
    Entity e;
    ASSERT_TRUE(e.IsActive());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Entity_SetActive) {
    Entity e;
    e.SetActive(false);
    ASSERT_FALSE(e.IsActive());
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Entity_PositionThroughTransform) {
    Entity e;
    e.SetPosition(100, 200);

    Point p = e.GetPosition();
    ASSERT_EQUAL(p.x, 100);
    ASSERT_EQUAL(p.y, 200);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Entity_ScaleThroughTransform) {
    Entity e;
    e.SetScale(3.0f);
    ASSERT_FLOAT_NEAR(e.GetScale(), 3.0f, 0.001f);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Entity_RotationThroughTransform) {
    Entity e;
    e.SetRotation(90.0f);
    ASSERT_FLOAT_NEAR(e.GetRotation(), 90.0f, 0.001f);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(Entity_TransformReference_IsConsistent) {
    Entity e;
    e.SetPosition(50, 60);

    const Transform& t = e.GetTransform();
    ASSERT_EQUAL(t.position.x, 50);
    ASSERT_EQUAL(t.position.y, 60);
    return TestResult{__FUNCTION__, true, ""};
}
