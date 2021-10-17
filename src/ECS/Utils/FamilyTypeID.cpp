#include "ECS/Utils/FamilyTypeID.h"

namespace ECS {
    class IEntity;
    class IComponent;
    class ISystem;

    namespace Utils::Internal {
        template
        class FamilyTypeID<IEntity>;

        template
        class FamilyTypeID<IComponent>;

        template
        class FamilyTypeID<ISystem>;
    }
}
