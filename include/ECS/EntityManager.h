#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "ECS/API.h"
#include "ECS/IEntity.h"
#include "Engine/Engine.h"
#include "ECS/Utils/Handle.h"
#include <boost/noncopyable.hpp>
#include "ECS/ComponentManager.h"
#include "Memory/Allocator/MemoryChunkAllocator.h"

#pragma warning(push)

// warning C4291: 'void *operator new(::size_t,void *) throw()': no matching operator delete found; memory will not be freed if initialization throws an exception
#pragma warning(disable: 4291)

namespace ECS {
    using EntityHandleTable = Utils::HandleTable<IEntity, EntityId>;

    class EntityManager : boost::noncopyable {
        class IEntityContainer : boost::noncopyable {
        public:
            virtual ~IEntityContainer() = default;

            virtual void DestroyEntity(IEntity *object) = 0;
        };

        template<class T>
        class EntityContainer : public ::Memory::Allocator::MemoryChunkAllocator<T, ENITY_T_CHUNK_SIZE>, public IEntityContainer {
        public:
            EntityContainer() : ::Memory::Allocator::MemoryChunkAllocator<T, ENITY_T_CHUNK_SIZE>("EntityManager") {}
            ~EntityContainer() override = default;

            void DestroyEntity(IEntity *object) override
            {
                object->~IEntity();
                this->DestroyObject(object);
            }
        };

    private:
        using EntityRegistry = std::unordered_map<EntityTypeId, IEntityContainer *>;
        EntityRegistry entityRegistry;

        using PendingDestroyedEntities = std::vector<EntityId>;
        PendingDestroyedEntities pendingDestroyedEntities;
        size_t numPendingDestroyedEntities;

        ComponentManager *componentManagerInstance;

        EntityHandleTable entityHandleTable;

        template<class T>
        inline EntityContainer<T> *GetEntityContainer()
        {
            static const EntityTypeId EID = T::STATIC_ENTITY_TYPE_ID;

            auto it = entityRegistry.find(EID);
            EntityContainer<T> *ec = nullptr;

            if (it == entityRegistry.end())
            {
                ec = new EntityContainer<T>();
                entityRegistry[EID] = ec;
            }
            else
                ec = (EntityContainer<T> *)it->second;

            return ec;
        }

        EntityId AcquireEntityId(IEntity *entity);
        void ReleaseEntityId(EntityId id);

    public:
        explicit EntityManager(ComponentManager *componentManagerInstance);
        ~EntityManager();

        template<class T, class... ARGS>
        EntityId CreateEntity(ARGS &&...args)
        {
            void *memory = GetEntityContainer<T>()->CreateObject();

            ECS::EntityId entityId = AcquireEntityId((T *)memory);

            IEntity *entity = new(memory)T(std::forward<ARGS>(args)...);

            ((T *)memory)->entityId = entityId;
            ((T *)memory)->componentManagerInstance = componentManagerInstance;

            return entityId;
        }

        void DestroyEntity(EntityId entityId)
        {
            if (entityId == INVALID_ENTITY_ID)
                return;

            IEntity *entity = entityHandleTable[entityId];

            if (!entity->destroyed)
            {
                if (numPendingDestroyedEntities < pendingDestroyedEntities.size())
                    pendingDestroyedEntities[numPendingDestroyedEntities++] = entityId;
                else
                {
                    pendingDestroyedEntities.push_back(entityId);
                    ++numPendingDestroyedEntities;
                }

                entity->onDelete();
                entity->OnDelete();
            }
        }

        inline IEntity *GetEntity(EntityId entityId)
        {
            if (entityId == ECS::INVALID_ENTITY_ID)
                return nullptr;

            return entityHandleTable[entityId];
        }

        [[nodiscard]] inline EntityId GetEntityId(EntityId::value_type index) const
        {
            return entityHandleTable[index];
        }

        void RemoveDestroyedEntities();
    };
}

#pragma warning(pop)

#endif
