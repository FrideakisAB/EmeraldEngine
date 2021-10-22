#ifndef IEVENTLISTENER_H
#define IEVENTLISTENER_H

#include <list>
#include "ECS/API.h"
#include "Engine/Engine.h"
#include "ECS/Event/EventDelegate.h"

namespace ECS::Event {
    class IEventListener {
        using RegisteredCallbacks = std::list<Internal::IEventDelegate *>;

    private:
        RegisteredCallbacks registeredCallbacks;

    public:
        IEventListener() = default;
        virtual ~IEventListener();

        template<class E, class C>
        inline void RegisterEventCallback(void(C::*Callback)(const E *const))
        {
            Internal::IEventDelegate *eventDelegate = new Internal::EventDelegate<C, E>(static_cast<C *>(this), Callback);

            registeredCallbacks.push_back(eventDelegate);
            Engine::Get().template SubscribeEvent<E>(eventDelegate);
        }

        template<class E, class C>
        inline void UnregisterEventCallback(void(C::*Callback)(const E *const))
        {
            Internal::EventDelegate<C, E> delegate(static_cast<C *>(this), Callback);

            for (auto callback : registeredCallbacks)
            {
                if (callback->GetDelegateId() == delegate.GetDelegateId())
                {
                    registeredCallbacks.remove_if(
                        [&](const Internal::IEventDelegate *other) {
                            return other->operator==(callback);
                        }
                    );

                    Engine::Get().UnsubscribeEvent(&delegate);
                    break;
                }
            }
        }

        void UnregisterAllEventCallbacks();
    };
}

#endif
