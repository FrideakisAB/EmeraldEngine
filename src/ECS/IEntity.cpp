#include "ECS/IEntity.h"

#include "ECS/EntityManager.h"
#include "ECS/ComponentManager.h"
#include "ECS/Serialization/EntityFactory.h"
#include "ECS/Serialization/ComponentFactory.h"

namespace ECS {
    inline ChildProxy::ChildListIterator &ChildProxy::ChildListIterator::operator++()
    {
        currentEntity = currentEntity->GetNextParentChild();
        return *this;
    }

    ChildProxy::ChildProxy(IEntity *firstChild) :
            beginIterator(firstChild)
    {}

    ChildProxy::ChildListIterator ChildProxy::begin() const noexcept
    {
        return beginIterator;
    }

    ChildProxy::ChildListIterator ChildProxy::end() const noexcept
    {
        return ChildProxy::ChildListIterator(nullptr);
    }

    IEntity::IEntity() : localActive(true), globalActive(true), destroyed(false) {}

    void IEntity::recSetActive() const
    {
        for (auto &entity : GetChildIterable())
        {
            entity.globalActive = IsActive();
            entity.recSetActive();
        }
    }

    void IEntity::onDelete()
    {
        destroyed = true;
        if (parentId != INVALID_ENTITY_ID)
            Engine::Get().GetEntityManager().GetEntity(parentId)->RemoveChild(entityId);

        auto nextId = GetChildIterable().begin();
        auto end = GetChildIterable().end();
        while (nextId != end)
        {
            Engine::Get().GetEntityManager().DestroyEntity(nextId->entityId);
            nextId = GetChildIterable().begin();
        }
    }

    IEntity *IEntity::GetParent() const noexcept
    {
        return Engine::Get().GetEntityManager().GetEntity(parentId);
    }

    void IEntity::SetParent(IEntity *entity)
    {
        if (entity != nullptr)
            entity->AddChild(this);
        else if (parentId != INVALID_ENTITY_ID)
            Engine::Get().GetEntityManager().GetEntity(parentId)->RemoveChild(entityId);
    }

    void IEntity::SetParent(EntityId newParentId)
    {
        if (newParentId != INVALID_ENTITY_ID)
        {
            auto *entity = Engine::Get().GetEntityManager().GetEntity(newParentId);
            entity->AddChild(this);
        }
        else if (parentId != INVALID_ENTITY_ID)
            Engine::Get().GetEntityManager().GetEntity(parentId)->RemoveChild(entityId);
    }

    IEntity *IEntity::GetChild() const
    {
        return Engine::Get().GetEntityManager().GetEntity(leftChildId);
    }

    IEntity *IEntity::GetNextParentChild() const
    {
        return Engine::Get().GetEntityManager().GetEntity(rightEntityId);
    }

    ChildProxy IEntity::GetChildIterable() const noexcept
    {
        return ChildProxy(GetChild());
    }

    void IEntity::AddChild(EntityId newChildId)
    {
        if (newChildId != INVALID_ENTITY_ID)
        {
            auto *entity = Engine::Get().GetEntityManager().GetEntity(newChildId);
            AddChild(entity);
        }
    }

    void IEntity::AddChild(IEntity *entity)
    {
        if (entity != nullptr && entity->parentId != entityId && entity->entityId != entityId && !entity->destroyed)
        {
            entity->SetParent(INVALID_ENTITY_ID);

            ++childCount;

            entity->parentId = entityId;
            entity->rightEntityId = leftChildId;
            leftChildId = entity->entityId;
            entity->globalActive = IsActive();
            entity->recSetActive();
            componentManagerInstance->UpdateTreeComponents(entity->entityId);
        }
    }

    void IEntity::RemoveChild(EntityId childId)
    {
        if (childId != INVALID_ENTITY_ID && childId != entityId)
        {
            auto *entity = Engine::Get().GetEntityManager().GetEntity(childId);
            if (entity->parentId == entityId)
            {
                if (leftChildId == childId)
                {
                    leftChildId = entity->rightEntityId;
                }
                else
                {
                    auto targetEntity = GetChildIterable().begin();
                    auto end = GetChildIterable().end();
                    while (targetEntity != end)
                    {
                        if (targetEntity->rightEntityId == childId)
                            break;
                        ++targetEntity;
                    }

                    targetEntity->rightEntityId = entity->rightEntityId;
                }

                --childCount;

                entity->parentId = INVALID_ENTITY_ID;
                entity->rightEntityId = INVALID_ENTITY_ID;
                entity->globalActive = true;
                entity->recSetActive();
                componentManagerInstance->UpdateTreeComponents(entity->entityId);
            }
        }
    }

    void IEntity::SetActive(bool active)
    {
        if (localActive == active)
            return;

        if (!active)
            OnDisable();
        else
            OnEnable();

        localActive = active;

        if (globalActive == true || parentId == INVALID_ENTITY_ID)
            recSetActive();
    }

    json IEntity::SerializeComponents() const
    {
        json ret;
        ret["cmps"] = {""};

        const size_t NUM_COMPONENTS = componentManagerInstance->entityComponentMap[0].size();
        for (ComponentTypeId componentTypeId = 0; componentTypeId < NUM_COMPONENTS; ++componentTypeId)
        {
            const ECS::ComponentId componentId = componentManagerInstance->entityComponentMap[GetEntityId().index][componentTypeId];
            if (componentId == INVALID_COMPONENT_ID)
                continue;

            if (IComponent *component = componentManagerInstance->componentLut[componentId]; component != nullptr)
            {
                json cmp = component->SerializeObj();
                ret[std::string(cmp["cmpName"])] = cmp;
                ret["cmps"].insert(ret["cmps"].begin(), std::string(cmp["cmpName"]));
            }
        }

        return ret;
    }

    void IEntity::UnSerializeComponents(const json &j)
    {
        for (auto &elm : j["cmps"])
        {
            if (elm != "")
            {
                IComponent *component = Engine::Get().GetComponentFactory().Add(std::string(elm), this);
                component->UnSerializeObj(j[std::string(elm)]);
            }
        }
    }

    json IEntity::SerializeObj() const
    {
        json data;

        data["components"] = SerializeComponents();
        data["childCount"] = childCount;
        data["localActive"] = localActive;

        u32 i = 0;
        for (auto &child : GetChildIterable())
        {
            data["child"][i] = child.SerializeObj();
            ++i;
        }

        return data;
    }

    void IEntity::UnSerializeObj(const json &j)
    {
        auto &EF = Engine::Get().GetEntityFactory();
        auto &EM = Engine::Get().GetEntityManager();

        UnSerializeComponents(j["components"]);

        localActive = j["localActive"].get<u8>();

        size_t oldChildCount = j["childCount"];
        for (size_t i = 0; i < oldChildCount; ++i)
        {
            IEntity *child = EM.GetEntity(EF.Create(j["child"][i]["base"]["ESID"]));
            AddChild(child);
            child->UnSerializeObj(j["child"][i]);
        }
    }
}
