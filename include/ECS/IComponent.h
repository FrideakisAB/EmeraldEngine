#ifndef ICOMPONENT_H
#define ICOMPONENT_H

#include "ECS/IEntity.h"
#include "ECS/Serialization/ISerialize.h"

namespace ECS {
    using ComponentId = ObjectID;
    using ComponentTypeId = TypeID;

    static const ComponentId INVALID_COMPONENT_ID = INVALID_OBJECT_ID;

    class IComponent : public ISerialize {
        friend class ComponentManager;

    protected:
        ComponentId hashValue;
        ComponentId componentId;
        EntityId owner;

        u8 enabled: 1;
        u8 treeLock: 1;
        u8 reserve: 6;

        virtual void UpdateTree() {};
        virtual void OnEnable() {}
        virtual void OnDisable() {}

    public:
        IComponent();
        virtual ~IComponent() = default;

        inline bool operator==(const IComponent &other) const noexcept { return hashValue == other.hashValue; }
        inline bool operator!=(const IComponent &other) const noexcept { return hashValue != other.hashValue; }

        [[nodiscard]] inline ComponentId GetComponentId() const noexcept { return componentId; }

        [[nodiscard]] inline EntityId GetOwner() const noexcept { return owner; }

        void SetActive(bool active);

        [[nodiscard]] inline bool IsActive() const noexcept { return enabled; }
    };
}

#endif
