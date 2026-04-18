#include "../Core/EventBus.h"
#include "../Core/Events.h"
#include "../../Tests/SimpleTest.h"

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
