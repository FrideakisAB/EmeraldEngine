#include "ECS/ComponentManager.h"

namespace ECS {
    ComponentManager::ComponentManager()
    {
        entityComponentMap.resize(ENITY_LUT_GROW);
        for (auto i = 0; i < ENITY_LUT_GROW; ++i)
            entityComponentMap[i].resize(Utils::Internal::FamilyTypeID<IComponent>::GetCount(), INVALID_COMPONENT_ID);
    }

    ComponentManager::~ComponentManager()
    {
        for (auto cc : componentContainerRegistry)
        {
            delete cc.second;
            cc.second = nullptr;
        }
    }

    ComponentId ComponentManager::AcquireComponentId(IComponent *component)
    {
        int i = 0;
        for (; i < componentLut.size(); ++i)
        {
            if (this->componentLut[i] == nullptr)
            {
                this->componentLut[i] = component;
                return i;
            }
        }

        componentLut.resize(componentLut.size() + COMPONENT_LUT_GROW, nullptr);

        componentLut[i] = component;
        return i;
    }

    void ComponentManager::ReleaseComponentId(ComponentId id)
    {
        if (id != INVALID_COMPONENT_ID && id < componentLut.size())
            componentLut[id] = nullptr;
    }

    void ComponentManager::MapEntityComponent(EntityId entityId, ComponentId componentId, ComponentTypeId componentTypeId)
    {
        const size_t NUM_COMPONENTS{Utils::Internal::FamilyTypeID<IComponent>::GetCount()};

        if ((entityComponentMap.size() - 1) < entityId.index)
        {
            size_t oldSize = entityComponentMap.size();
            size_t newSize = oldSize + ENITY_LUT_GROW;

            entityComponentMap.resize(newSize);

            for (auto i = oldSize; i < newSize; ++i)
                entityComponentMap[i].resize(NUM_COMPONENTS, INVALID_COMPONENT_ID);
        }

        entityComponentMap[entityId.index][componentTypeId] = componentId;
    }

    void ComponentManager::UnmapEntityComponent(EntityId entityId, ComponentId componentId, ComponentTypeId componentTypeId)
    {
        assert(entityComponentMap[entityId.index][componentTypeId] == componentId && "FATAL: Entity Component ID mapping corruption!");

        entityComponentMap[entityId.index][componentTypeId] = INVALID_COMPONENT_ID;
        ReleaseComponentId(componentId);
    }
}
