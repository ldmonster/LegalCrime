#pragma once

#include <functional>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <shared_mutex>

namespace Engine {

    /// Lightweight type-safe publish/subscribe event bus.
    /// Thread-safe: uses shared_mutex (readers-writer lock) for handler maps.
    /// Usage:
    ///   EventBus bus;
    ///   bus.Subscribe<MyEvent>([](const MyEvent& e) { ... });
    ///   bus.Publish(MyEvent{ ... });
    class EventBus {
    public:
        template<typename TEvent>
        using Handler = std::function<void(const TEvent&)>;

        /// Subscribe to events of type TEvent. Returns a subscription ID.
        template<typename TEvent>
        size_t Subscribe(Handler<TEvent> handler) {
            std::unique_lock lock(m_mutex);
            auto& handlers = GetHandlersLocked<TEvent>();
            size_t id = m_nextId++;
            handlers.push_back({ id, std::move(handler) });
            return id;
        }

        /// Unsubscribe by subscription ID.
        template<typename TEvent>
        void Unsubscribe(size_t subscriptionId) {
            std::unique_lock lock(m_mutex);
            auto& handlers = GetHandlersLocked<TEvent>();
            handlers.erase(
                std::remove_if(handlers.begin(), handlers.end(),
                    [subscriptionId](const Subscription<TEvent>& s) {
                        return s.id == subscriptionId;
                    }),
                handlers.end()
            );
        }

        /// Publish an event to all subscribers.
        template<typename TEvent>
        void Publish(const TEvent& event) {
            std::shared_lock lock(m_mutex);
            auto key = std::type_index(typeid(TEvent));
            auto it = m_handlers.find(key);
            if (it == m_handlers.end()) return;
            auto& handlers = static_cast<HandlerListImpl<TEvent>*>(it->second.get())->subscriptions;
            for (auto& sub : handlers) {
                sub.handler(event);
            }
        }

        /// Remove all subscriptions.
        void Clear() {
            std::unique_lock lock(m_mutex);
            m_handlers.clear();
        }

    private:
        template<typename TEvent>
        struct Subscription {
            size_t id;
            Handler<TEvent> handler;
        };

        struct IHandlerListBase {
            virtual ~IHandlerListBase() = default;
        };

        template<typename TEvent>
        struct HandlerListImpl : IHandlerListBase {
            std::vector<Subscription<TEvent>> subscriptions;
        };

        /// Must be called with m_mutex held (unique or shared).
        template<typename TEvent>
        std::vector<Subscription<TEvent>>& GetHandlersLocked() {
            auto key = std::type_index(typeid(TEvent));
            auto it = m_handlers.find(key);
            if (it == m_handlers.end()) {
                auto list = std::make_unique<HandlerListImpl<TEvent>>();
                auto* ptr = list.get();
                m_handlers[key] = std::move(list);
                return ptr->subscriptions;
            }
            return static_cast<HandlerListImpl<TEvent>*>(it->second.get())->subscriptions;
        }

        std::unordered_map<std::type_index, std::unique_ptr<IHandlerListBase>> m_handlers;
        mutable std::shared_mutex m_mutex;
        size_t m_nextId = 0;
    };

} // namespace Engine
