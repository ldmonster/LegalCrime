#pragma once

#include "../Engine/Core/Types.h"
#include "../Engine/Core/EventBus.h"
#include <cstdint>
#include <vector>

namespace LegalCrime {

    struct EntitySpawnedEvent {
        uint32_t entityId;
        Engine::TilePosition position;
    };

    struct EntityDestroyedEvent {
        uint32_t entityId;
    };

    struct EntityMovedEvent {
        uint32_t entityId;
        Engine::TilePosition from;
        Engine::TilePosition to;
    };

    struct SelectionChangedEvent {
        std::vector<uint32_t> selectedEntityIds;
    };

    struct CommandIssuedEvent {
        uint32_t entityId;
        Engine::TilePosition target;
    };

    // Global domain event bus for game-layer events.
    inline Engine::EventBus& DomainEventBus() {
        static Engine::EventBus bus;
        return bus;
    }

} // namespace LegalCrime
