#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <vector>
#include "ECS/API.h"
#include "ECS/Event/IEvent.h"
#include <boost/noncopyable.hpp>
#include "Memory/GlobalMemoryUser.h"
#include "ECS/Event/EventDispatcher.h"
#include "Memory/Allocator/LinearAllocator.h"

class Engine;

namespace ECS::Event {
    class EventHandler : ::Memory::GlobalMemoryUser, boost::noncopyable {
        friend Engine;

        using EventDispatcherMap = std::unordered_map<EventTypeId, Internal::IEventDispatcher *>;
        using EventStorage = std::vector<IEvent *>;
        using EventMemoryAllocator = Memory::Allocator::LinearAllocator;

    private:
        EventDispatcherMap eventDispatcherMap;
        EventMemoryAllocator *eventMemoryAllocator;
        EventStorage eventStorage;

        template<class E>
        inline void AddEventCallback(Internal::IEventDelegate *const eventDelegate)
        {
            EventTypeId ETID = E::STATIC_EVENT_TYPE_ID;

            EventDispatcherMap::const_iterator iter = eventDispatcherMap.find(ETID);
            if (iter == eventDispatcherMap.end())
            {
                std::pair<EventTypeId, Internal::IEventDispatcher *> kvp(ETID, new Internal::EventDispatcher<E>());

                kvp.second->AddEventCallback(eventDelegate);

                eventDispatcherMap.insert(kvp);
            }
            else
            {
                eventDispatcherMap[ETID]->AddEventCallback(eventDelegate);
            }
        }

        inline void RemoveEventCallback(Internal::IEventDelegate *eventDelegate)
        {
            auto typeId = eventDelegate->GetStaticEventTypeId();
            EventDispatcherMap::const_iterator iter = eventDispatcherMap.find(typeId);
            if (iter != eventDispatcherMap.end())
                eventDispatcherMap[typeId]->RemoveEventCallback(eventDelegate);
        }

    public:
        EventHandler();
        ~EventHandler() override;

        inline void ClearEventBuffer()
        {
            eventMemoryAllocator->Clear();
            eventStorage.clear();
        }

        inline void ClearEventDispatcher()
        {
            eventDispatcherMap.clear();
        }

        template<class E, class... ARGS>
        void Send(ARGS &&...eventArgs)
        {
            static_assert(std::is_trivially_copyable<E>::value, "Event is not trivially copyable.");

            void *memory = eventMemoryAllocator->Allocate(sizeof(E), alignof(E));

            if (memory != nullptr)
                eventStorage.push_back(new(memory)E(std::forward<ARGS>(eventArgs)...));
            else
                logger->Error("Memory for Event is out! Event no send");
        }

        void DispatchEvents()
        {
            size_t lastIndex = eventStorage.size();
            size_t thisIndex = 0;

            while (thisIndex < lastIndex)
            {
                auto event = eventStorage[thisIndex++];
                if (event == nullptr)
                {
                    logger->Error("Event corrupted");
                    continue;
                }

                auto it = eventDispatcherMap.find(event->GetEventTypeId());
                if (it == eventDispatcherMap.end())
                    continue;

                it->second->Dispatch(event);
                lastIndex = eventStorage.size();
            }

            ClearEventBuffer();
        }
    };
}

#endif
