#include "ECS/EntityManager.h"

namespace ECS {
    EntityManager::EntityManager(ComponentManager *componentManagerInstance) :
            pendingDestroyedEntities(1024),
            numPendingDestroyedEntities(0),
            componentManagerInstance(componentManagerInstance)
    {}

    EntityManager::~EntityManager()
    {
        RemoveDestroyedEntities();
        for (auto ec : entityRegistry)
        {
            delete ec.second;
            ec.second = nullptr;
        }
    }

    EntityId EntityManager::AcquireEntityId(IEntity *entity)
    {
        return entityHandleTable.AcquireHandle(entity);
    }

    void EntityManager::ReleaseEntityId(EntityId id)
    {
        entityHandleTable.ReleaseHandle(id);
    }

    void EntityManager::RemoveDestroyedEntities()
    {
        for (size_t i = 0; i < numPendingDestroyedEntities; ++i)
        {
            EntityId entityId = pendingDestroyedEntities[i];
            IEntity *entity = entityHandleTable[entityId];

            const EntityTypeId ETID = entity->GetStaticEntityTypeId();

            if (auto it = entityRegistry.find(ETID); it != entityRegistry.end())
            {
                componentManagerInstance->RemoveAllComponents(entityId);
                it->second->DestroyEntity(entity);
            }

            ReleaseEntityId(entityId);
        }

        numPendingDestroyedEntities = 0;
    }
}
