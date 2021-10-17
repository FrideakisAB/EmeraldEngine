#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <list>
#include <algorithm>
#include "ECS/API.h"
#include "ECS/Event/IEventDispatcher.h"

namespace ECS::Event::Internal {
    template<class T>
    class EventDispatcher : public IEventDispatcher {
        using EventDelegateList = std::list<IEventDelegate *>;
        using PendingRemoveDelegates = std::list<IEventDelegate *>;

    private:
        PendingRemoveDelegates pendingRemoveDelegates;
        EventDelegateList eventCallbacks;
        std::atomic_bool locked;

    public:
        EventDispatcher() : locked(false) {}

        ~EventDispatcher() override
        {
            pendingRemoveDelegates.clear();
            eventCallbacks.clear();
        }

        inline void Dispatch(IEvent *event) override
        {
            locked = true;
            {
                if (!pendingRemoveDelegates.empty())
                {
                    for (auto EC : pendingRemoveDelegates)
                    {
                        auto result = std::find_if(eventCallbacks.begin(), eventCallbacks.end(),
                            [&](const IEventDelegate *other) {
                                return other->operator==(EC);
                            });

                        if (result != eventCallbacks.end())
                        {
                            auto *memory = (IEventDelegate *)(*result);

                            eventCallbacks.erase(result);

                            delete memory;
                            memory = nullptr;
                        }
                    }
                    pendingRemoveDelegates.clear();
                }

                for (auto EC : eventCallbacks)
                {
                    assert(EC != nullptr && "Invalid event callback.");
                    EC->Invoke(event);
                }
            }
            locked = false;
        }

        void AddEventCallback(IEventDelegate *const eventDelegate) override
        {
            auto result = std::find_if(pendingRemoveDelegates.begin(), pendingRemoveDelegates.end(),
                [&](const IEventDelegate *other) {
                    return other->operator==(eventDelegate);
                });

            if (result != pendingRemoveDelegates.end())
            {
                pendingRemoveDelegates.erase(result);
                return;
            }

            eventCallbacks.push_back(eventDelegate);
        }

        void RemoveEventCallback(IEventDelegate *eventDelegate) override
        {
            if (!locked)
            {
                auto result = std::find_if(eventCallbacks.begin(), eventCallbacks.end(),
                    [&](const IEventDelegate *other) {
                        return other->operator==(eventDelegate);
                    });

                if (result != eventCallbacks.end())
                {
                    auto *memory = (IEventDelegate *)(*result);

                    eventCallbacks.erase(result);

                    delete memory;
                }
            }
            else
            {
                auto result = std::find_if(eventCallbacks.begin(), eventCallbacks.end(),
                    [&](const IEventDelegate *other) {
                        return other->operator==(eventDelegate);
                    });

                assert(result != eventCallbacks.end() && "");
                pendingRemoveDelegates.push_back(*result);
            }
        }

        [[nodiscard]] inline size_t GetEventCallbackCount() const noexcept override { return eventCallbacks.size(); }
    };
}

#endif
