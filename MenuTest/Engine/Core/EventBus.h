#pragma once

#include <functional>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>

namespace Engine {

    /// Lightweight type-safe publish/subscribe event bus.
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
            auto& handlers = GetHandlers<TEvent>();
            size_t id = m_nextId++;
            handlers.push_back({ id, std::move(handler) });
            return id;
        }

        /// Unsubscribe by subscription ID.
        template<typename TEvent>
        void Unsubscribe(size_t subscriptionId) {
            auto& handlers = GetHandlers<TEvent>();
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
            auto& handlers = GetHandlers<TEvent>();
            for (auto& sub : handlers) {
                sub.handler(event);
            }
        }

        /// Remove all subscriptions.
        void Clear() {
            m_handlers.clear();
        }

    private:
        template<typename TEvent>
        struct Subscription {
            size_t id;
            Handler<TEvent> handler;
        };

        template<typename TEvent>
        struct HandlerList {
            std::vector<Subscription<TEvent>> subscriptions;
        };

        struct IHandlerListBase {
            virtual ~IHandlerListBase() = default;
        };

        template<typename TEvent>
        struct HandlerListImpl : IHandlerListBase {
            std::vector<Subscription<TEvent>> subscriptions;
        };

        template<typename TEvent>
        std::vector<Subscription<TEvent>>& GetHandlers() {
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
        size_t m_nextId = 0;
    };

} // namespace Engine
