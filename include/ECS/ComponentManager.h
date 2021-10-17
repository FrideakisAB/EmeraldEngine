#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "ECS/API.h"
#include "ECS/IComponent.h"
#include <boost/noncopyable.hpp>
#include "ECS/Utils/FamilyTypeID.h"
#include "Memory/GlobalMemoryUser.h"
#include "Memory/Allocator/MemoryChunkAllocator.h"

namespace ECS {
    class ComponentManager final : ::Memory::GlobalMemoryUser, boost::noncopyable {
        friend class IComponent;
        friend class IEntity;

        class IComponentContainer : public boost::noncopyable {
        public:
            virtual ~IComponentContainer() = default;

            virtual void DestroyComponent(IComponent *object) = 0;
        };

        template<class T>
        class ComponentContainer : public ::Memory::Allocator::MemoryChunkAllocator<T, COMPONENT_T_CHUNK_SIZE>, public IComponentContainer {
        public:
            ComponentContainer() : ::Memory::Allocator::MemoryChunkAllocator<T, COMPONENT_T_CHUNK_SIZE>("ComponentManager") {}
            ~ComponentContainer() override = default;

            void DestroyComponent(IComponent *object) override
            {
                object->~IComponent();
                this->DestroyObject(object);
            }
        };

    private:
        using ComponentContainerRegistry = std::unordered_map<ComponentTypeId, IComponentContainer *>;
        ComponentContainerRegistry componentContainerRegistry;

        template<class T>
        inline ComponentContainer<T> *GetComponentContainer()
        {
            static const ComponentTypeId CID = T::STATIC_COMPONENT_TYPE_ID;

            auto it = componentContainerRegistry.find(CID);
            ComponentContainer<T> *cc = nullptr;

            if (it != componentContainerRegistry.end())
                cc = static_cast<ComponentContainer<T> *>(it->second);
            else
            {
                cc = new ComponentContainer<T>();
                componentContainerRegistry[CID] = cc;
            }

            return cc;
        }

        using ComponentLookupTable = std::vector<IComponent *>;
        ComponentLookupTable componentLut;

        using EntityComponentMap = std::vector<std::vector<ComponentId>>;
        EntityComponentMap entityComponentMap;


        ComponentId AcquireComponentId(IComponent *component);
        void ReleaseComponentId(ComponentId id);

        void MapEntityComponent(EntityId entityId, ComponentId componentId, ComponentTypeId componentTypeId);
        void UnmapEntityComponent(EntityId entityId, ComponentId componentId, ComponentTypeId componentTypeId);

    public:
        template<class T>
        using TComponentIterator = typename ComponentContainer<T>::iterator;

        template<class T>
        class ComponentProxy {
        private:
            ComponentContainer<T> *container;

        public:
            explicit ComponentProxy(ComponentContainer<T> *container) : container(container) {}

            inline TComponentIterator<T> begin()
            {
                return container->begin();
            }

            inline TComponentIterator<T> end()
            {
                return container->end();
            }
        };

        ComponentManager();
        ~ComponentManager() final;

        template<class T, class ...ARGS>
        T *AddComponent(const EntityId entityId, ARGS &&...args)
        {
            constexpr std::hash<ComponentId> ENTITY_COMPONENT_ID_HASHER{std::hash<ComponentId>()};

            static const ComponentTypeId CTID = T::STATIC_COMPONENT_TYPE_ID;

            const ComponentId cmpId = entityComponentMap[entityId.index][CTID];
            if (cmpId != INVALID_COMPONENT_ID)
            {
                if (IComponent *oldCmp = componentLut[cmpId]; oldCmp != nullptr)
                    RemoveComponent<T>(entityId);
            }

            void *memory = GetComponentContainer<T>()->CreateObject();

            ComponentId componentId = AcquireComponentId((T*)memory);
            ((T*)memory)->componentId = componentId;

            IComponent *component = new(memory)T(std::forward<ARGS>(args)...);

            component->owner = entityId;
            component->hashValue = ENTITY_COMPONENT_ID_HASHER(entityId) ^ (ENTITY_COMPONENT_ID_HASHER(componentId) << 1);

            MapEntityComponent(entityId, componentId, CTID);

            return static_cast<T*>(component);
        }

        template<class T>
        void RemoveComponent(const EntityId entityId)
        {
            static const ComponentTypeId CTID = T::STATIC_COMPONENT_TYPE_ID;
            const ComponentId componentId = entityComponentMap[entityId.index][CTID];

            IComponent *component = componentLut[componentId];

            if (component == nullptr)
                return;

            GetComponentContainer<T>()->DestroyComponent(component);

            UnmapEntityComponent(entityId, componentId, CTID);
        }

        void RemoveComponent(const EntityId entityId, const ComponentId cmpId)
        {
            if (cmpId != ECS::INVALID_COMPONENT_ID)
            {
                ComponentTypeId componentTypeId = 0;
                auto &entityComponents = entityComponentMap[entityId.index];
                auto It = std::find_if(entityComponents.begin(), entityComponents.end(),
                    [cmpId, &componentTypeId](const ComponentId &id) {
                        if (id == cmpId)
                            return true;

                        ++componentTypeId;
                        return false;
                });

                if (It != entityComponents.end())
                {
                    IComponent *component = componentLut[*It];
                    if (component != nullptr)
                    {
                        auto it = componentContainerRegistry.find(componentTypeId);
                        if (it != componentContainerRegistry.end())
                            it->second->DestroyComponent(component);

                        UnmapEntityComponent(entityId, *It, componentTypeId);
                    }
                }
            }
        }

        void RemoveAllComponents(const EntityId entityId)
        {
            const size_t NUM_COMPONENTS = entityComponentMap[0].size();

            for (ComponentTypeId componentTypeId = 0; componentTypeId < NUM_COMPONENTS; ++componentTypeId)
            {
                const ComponentId componentId = entityComponentMap[entityId.index][componentTypeId];
                if (componentId == INVALID_COMPONENT_ID)
                    continue;

                IComponent *component = componentLut[componentId];
                if (component != nullptr)
                {
                    auto it = componentContainerRegistry.find(componentTypeId);
                    if (it != componentContainerRegistry.end())
                        it->second->DestroyComponent(component);

                    UnmapEntityComponent(entityId, componentId, componentTypeId);
                }
            }
        }

        void UpdateTreeComponents(const EntityId entityId)
        {
            const size_t NUM_COMPONENTS = entityComponentMap[0].size();

            for (ComponentTypeId componentTypeId = 0; componentTypeId < NUM_COMPONENTS; ++componentTypeId)
            {
                const ComponentId componentId = entityComponentMap[entityId.index][componentTypeId];
                if (componentId == INVALID_COMPONENT_ID)
                    continue;

                if (IComponent *component = componentLut[componentId]; component != nullptr && component->treeLock)
                    component->UpdateTree();
            }
        }

        template<class T>
        T *GetComponent(const EntityId entityId)
        {
            if (entityId == INVALID_ENTITY_ID)
                return nullptr;

            static const ComponentTypeId CTID = T::STATIC_COMPONENT_TYPE_ID;
            const ComponentId componentId = entityComponentMap[entityId.index][CTID];

            if (componentId == INVALID_COMPONENT_ID)
                return nullptr;

            return static_cast<T*>(componentLut[componentId]);
        }

        template<class T>
        ComponentProxy<T> GetIterable()
        {
            return ComponentProxy<T>(GetComponentContainer<T>());
        }

        template<class T>
        inline TComponentIterator<T> begin()
        {
            return GetComponentContainer<T>()->begin();
        }

        template<class T>
        inline TComponentIterator<T> end()
        {
            return GetComponentContainer<T>()->end();
        }
    };
}

#endif
