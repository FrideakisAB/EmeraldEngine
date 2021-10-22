#ifndef IEVENT_H
#define IEVENT_H

#include "ECS/API.h"

namespace ECS::Event {
    using EventTypeId = TypeID;
    using EventTimestamp = TimeStamp;

    static const EventTypeId INVALID_EVENT_TYPE = INVALID_TYPE_ID;

    class IEvent {
    private:
        EventTypeId typeId;
        EventTimestamp timeCreated;

    public:
        explicit IEvent(EventTypeId typeId);

        [[nodiscard]] inline EventTypeId GetEventTypeId() const noexcept { return typeId; }
        [[nodiscard]] inline EventTimestamp GetTimeCreated() const noexcept { return timeCreated; }
    };
}

#endif
