#include "ECS/Event/EventHandler.h"

namespace ECS::Event {
    EventHandler::EventHandler()
    {
        eventMemoryAllocator = new EventMemoryAllocator(ECS_EVENT_MEMORY_BUFFER_SIZE, AllocateGlobal(ECS_EVENT_MEMORY_BUFFER_SIZE, "EventHandler"));
        eventStorage.reserve(1024);
    }

    EventHandler::~EventHandler()
    {
        for (auto &it : eventDispatcherMap)
        {
            delete it.second;
            it.second = nullptr;
        }

        eventDispatcherMap.clear();

        FreeGlobal((void *)eventMemoryAllocator->GetMemoryAddress0());

        delete eventMemoryAllocator;
        eventMemoryAllocator = nullptr;
    }
}
