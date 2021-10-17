#include "ECS/Event/IEvent.h"

#include "Engine/Engine.h"
#include "ECS/Utils/Timer.h"

namespace ECS::Event {
    IEvent::IEvent(EventTypeId typeId) :
            typeId(typeId)
    {
        timeCreated = Engine::Get().GetTimer().GetTimeStamp();
    }
}
