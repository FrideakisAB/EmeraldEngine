#ifndef ENTITY_H
#define ENTITY_H

#include "ECS/IEntity.h"
#include "ECS/Utils/FamilyTypeID.h"

namespace ECS {
    template<class E>
    class Entity : public IEntity {
    public:
        static const EntityTypeId STATIC_ENTITY_TYPE_ID;

        ~Entity() override = default;

        void operator delete[](void *) {}
        void operator delete(void *) {}

        [[nodiscard]] const EntityTypeId GetStaticEntityTypeId() const final { return STATIC_ENTITY_TYPE_ID; }
    };

    template<class E>
    const EntityTypeId Entity<E>::STATIC_ENTITY_TYPE_ID = Utils::Internal::FamilyTypeID<IEntity>::Get<E>();
}

#endif
