#ifndef EVENTDELEGATE_H
#define EVENTDELEGATE_H

#include "Platform.h"

namespace ECS::Event {
    class IEvent;

    namespace Internal {
        using EventDelegateId = size_t;

        class IEventDelegate {
        public:
            virtual ~IEventDelegate() = default;
            virtual inline void Invoke(const IEvent *const event) = 0;
            [[nodiscard]] virtual inline EventDelegateId GetDelegateId() const = 0;
            [[nodiscard]] virtual inline u64 GetStaticEventTypeId() const = 0;
            virtual bool operator==(const IEventDelegate *other) const = 0;
            [[nodiscard]] virtual IEventDelegate *Clone() = 0;
        };

        template<class Class, class EventType>
        class EventDelegate : public IEventDelegate {
            typedef void(Class::*Callback)(const EventType *const);

            Class *receiver;
            Callback callback;

        public:
            EventDelegate(Class *receiver, Callback &callbackFunction) :
                    receiver(receiver), callback(callbackFunction)
            {}

            [[nodiscard]] IEventDelegate *Clone() override
            {
                return new EventDelegate(receiver, callback);
            }

            inline void Invoke(const IEvent *const event) override
            {
                (receiver->*callback)(reinterpret_cast<const EventType *const>(event));
            }

            [[nodiscard]] inline EventDelegateId GetDelegateId() const final
            {
                static const EventDelegateId DELEGATE_ID{typeid(Class).hash_code() ^ typeid(Callback).hash_code()};
                return DELEGATE_ID;
            }

            [[nodiscard]] inline u64 GetStaticEventTypeId() const override
            {
                static const u64 SEID{EventType::STATIC_EVENT_TYPE_ID};
                return SEID;
            }

            bool operator==(const IEventDelegate *other) const override
            {
                if (other == nullptr)
                    return false;
                if (GetDelegateId() != other->GetDelegateId())
                    return false;

                const auto *delegate = static_cast<const EventDelegate *>(other);
                return ((callback == delegate->callback) && (receiver == delegate->receiver));
            }
        };
    }
}

#endif
