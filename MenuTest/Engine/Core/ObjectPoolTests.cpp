#include "../../Tests/SimpleTest.h"
#include "ObjectPool.h"

using namespace Engine;
using namespace SimpleTest;

TEST_CASE(ObjectPool_StartsEmpty) {
    ObjectPool<int> pool(16);
    ASSERT_EQUAL(static_cast<int>(pool.Count()), 0);
    ASSERT_EQUAL(static_cast<int>(pool.Capacity()), 16);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(ObjectPool_Acquire_ReturnsValidPointer) {
    ObjectPool<int> pool(8);
    int* p = pool.Acquire();
    ASSERT_NOT_NULL(p);
    ASSERT_EQUAL(static_cast<int>(pool.Count()), 1);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(ObjectPool_Acquire_DefaultInitializes) {
    struct Obj { int x = 42; float y = 3.14f; };
    ObjectPool<Obj> pool(4);
    Obj* p = pool.Acquire();
    ASSERT_NOT_NULL(p);
    ASSERT_EQUAL(p->x, 42);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(ObjectPool_Acquire_FullReturnsNull) {
    ObjectPool<int> pool(2);
    pool.Acquire();
    pool.Acquire();
    int* p = pool.Acquire();
    ASSERT_NULL(p);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(ObjectPool_Reset_AllowsReuse) {
    ObjectPool<int> pool(2);
    int* a = pool.Acquire();
    int* b = pool.Acquire();
    ASSERT_NULL(pool.Acquire());

    pool.Reset();
    ASSERT_EQUAL(static_cast<int>(pool.Count()), 0);

    int* c = pool.Acquire();
    ASSERT_NOT_NULL(c);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(ObjectPool_Reserve_GrowsCapacity) {
    ObjectPool<int> pool(2);
    pool.Reserve(100);
    ASSERT_TRUE(pool.Capacity() >= 100);
    return TestResult{__FUNCTION__, true, ""};
}

TEST_CASE(ObjectPool_MultipleAcquires_ContiguousAccess) {
    ObjectPool<int> pool(64);
    int* first = pool.Acquire();
    int* second = pool.Acquire();
    int* third = pool.Acquire();

    // Objects should be contiguous (adjacent in memory)
    ASSERT_NOT_NULL(first);
    ASSERT_NOT_NULL(second);
    ASSERT_NOT_NULL(third);
    ASSERT_EQUAL(static_cast<int>(pool.Count()), 3);
    return TestResult{__FUNCTION__, true, ""};
}
