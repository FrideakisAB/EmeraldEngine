#include "ECS/Event/IEventListener.h"

#include "Engine/Engine.h"

namespace ECS::Event {
    IEventListener::~IEventListener()
    {
        UnregisterAllEventCallbacks();
    }

    void IEventListener::UnregisterAllEventCallbacks()
    {
        for (auto *callback : registeredCallbacks)
            Engine::Get().UnsubscribeEvent(callback);

        registeredCallbacks.clear();
    }
}
