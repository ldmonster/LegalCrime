#include "../Core/EventBus.h"
#include "../Core/Events.h"
#include "../../Tests/SimpleTest.h"
#include <thread>
#include <atomic>

#define PASS return SimpleTest::TestResult{__FUNCTION__, true, ""}

// ========== EventBus Tests ==========

TEST_CASE(EventBus_PublishWithNoSubscribers) {
    Engine::EventBus bus;
    // Should not crash
    bus.Publish(Engine::CharacterMovedEvent{ 1, {0,0}, {1,1} });
    PASS;
}

TEST_CASE(EventBus_SubscribeAndPublish) {
    Engine::EventBus bus;
    int callCount = 0;
    Engine::TilePosition receivedTo;

    bus.Subscribe<Engine::CharacterMovedEvent>([&](const Engine::CharacterMovedEvent& e) {
        callCount++;
        receivedTo = e.to;
    });

    bus.Publish(Engine::CharacterMovedEvent{ 1, {0,0}, {5,3} });

    ASSERT_EQUAL(callCount, 1);
    ASSERT_EQUAL(receivedTo.row, static_cast<uint16_t>(5));
    ASSERT_EQUAL(receivedTo.col, static_cast<uint16_t>(3));
    PASS;
}

TEST_CASE(EventBus_MultipleSubscribers) {
    Engine::EventBus bus;
    int count1 = 0, count2 = 0;

    bus.Subscribe<Engine::CharacterSelectedEvent>([&](const Engine::CharacterSelectedEvent&) { count1++; });
    bus.Subscribe<Engine::CharacterSelectedEvent>([&](const Engine::CharacterSelectedEvent&) { count2++; });

    bus.Publish(Engine::CharacterSelectedEvent{ 42 });

    ASSERT_EQUAL(count1, 1);
    ASSERT_EQUAL(count2, 1);
    PASS;
}

TEST_CASE(EventBus_DifferentEventTypes_Independent) {
    Engine::EventBus bus;
    int movedCount = 0, selectedCount = 0;

    bus.Subscribe<Engine::CharacterMovedEvent>([&](const Engine::CharacterMovedEvent&) { movedCount++; });
    bus.Subscribe<Engine::CharacterSelectedEvent>([&](const Engine::CharacterSelectedEvent&) { selectedCount++; });

    bus.Publish(Engine::CharacterMovedEvent{ 1, {0,0}, {1,1} });

    ASSERT_EQUAL(movedCount, 1);
    ASSERT_EQUAL(selectedCount, 0);
    PASS;
}

TEST_CASE(EventBus_Unsubscribe) {
    Engine::EventBus bus;
    int callCount = 0;

    size_t id = bus.Subscribe<Engine::CharacterSelectedEvent>([&](const Engine::CharacterSelectedEvent&) { callCount++; });

    bus.Publish(Engine::CharacterSelectedEvent{ 1 });
    ASSERT_EQUAL(callCount, 1);

    bus.Unsubscribe<Engine::CharacterSelectedEvent>(id);
    bus.Publish(Engine::CharacterSelectedEvent{ 2 });
    ASSERT_EQUAL(callCount, 1);  // Should not increase
    PASS;
}

TEST_CASE(EventBus_Clear) {
    Engine::EventBus bus;
    int callCount = 0;

    bus.Subscribe<Engine::CharacterMovedEvent>([&](const Engine::CharacterMovedEvent&) { callCount++; });
    bus.Clear();
    bus.Publish(Engine::CharacterMovedEvent{ 1, {0,0}, {1,1} });

    ASSERT_EQUAL(callCount, 0);
    PASS;
}

TEST_CASE(EventBus_SceneTransitionEvent) {
    Engine::EventBus bus;
    std::string target;

    bus.Subscribe<Engine::SceneTransitionEvent>([&](const Engine::SceneTransitionEvent& e) {
        target = e.targetScene;
    });

    bus.Publish(Engine::SceneTransitionEvent{ "MainMenu" });
    ASSERT_TRUE(target == "MainMenu");
    PASS;
}

// ========== Thread Safety Tests ==========

TEST_CASE(EventBus_ConcurrentPublish_NoDataRace) {
    Engine::EventBus bus;
    std::atomic<int> callCount{0};

    bus.Subscribe<Engine::CharacterMovedEvent>([&](const Engine::CharacterMovedEvent&) {
        callCount.fetch_add(1, std::memory_order_relaxed);
    });

    constexpr int ITERATIONS = 100;
    auto publisher = [&]() {
        for (int i = 0; i < ITERATIONS; ++i) {
            bus.Publish(Engine::CharacterMovedEvent{ 1, {0,0}, {1,1} });
        }
    };

    std::thread t1(publisher);
    std::thread t2(publisher);
    t1.join();
    t2.join();

    ASSERT_EQUAL(callCount.load(), ITERATIONS * 2);
    PASS;
}

TEST_CASE(EventBus_ConcurrentSubscribeAndPublish) {
    Engine::EventBus bus;
    std::atomic<int> callCount{0};

    // Pre-subscribe one handler
    bus.Subscribe<Engine::CharacterSelectedEvent>([&](const Engine::CharacterSelectedEvent&) {
        callCount.fetch_add(1, std::memory_order_relaxed);
    });

    // One thread publishes, another subscribes
    std::thread publisher([&]() {
        for (int i = 0; i < 50; ++i) {
            bus.Publish(Engine::CharacterSelectedEvent{ static_cast<uint32_t>(i) });
        }
    });

    std::thread subscriber([&]() {
        for (int i = 0; i < 10; ++i) {
            bus.Subscribe<Engine::CharacterSelectedEvent>([&](const Engine::CharacterSelectedEvent&) {
                callCount.fetch_add(1, std::memory_order_relaxed);
            });
        }
    });

    publisher.join();
    subscriber.join();

    // At least the pre-subscribed handler should have been called
    ASSERT_TRUE(callCount.load() >= 50);
    PASS;
}
