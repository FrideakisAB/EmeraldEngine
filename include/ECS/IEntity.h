#ifndef IENTITY_H
#define IENTITY_H

#include "ECS/API.h"
#include "Utils/json.h"
#include "ECS/Utils/Handle.h"
#include "ECS/Serialization/ISerialize.h"

namespace ECS {
    using EntityTypeId = TypeID;
    using EntityId = Utils::Handle64;

    static const EntityId INVALID_ENTITY_ID = Utils::Handle64::INVALID_HANDLE;

    class IEntity;

    class ChildProxy {
    public:
        class ChildListIterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = IEntity;
            using difference_type = std::ptrdiff_t;
            using pointer = value_type*;
            using reference = value_type&;

        private:
            IEntity *currentEntity;

        public:
            explicit ChildListIterator(IEntity *currentEntity) :
                currentEntity(currentEntity)
            {}

            inline ChildListIterator &operator++();

            [[nodiscard]] inline reference operator*() const { return *currentEntity; }
            [[nodiscard]] inline pointer operator->() const { return currentEntity; }

            [[nodiscard]] inline bool operator==(const ChildListIterator &other) const
            {
                return currentEntity == other.currentEntity;
            }

            [[nodiscard]] inline bool operator!=(const ChildListIterator &other) const
            {
                return currentEntity != other.currentEntity;
            }
        };

        ChildListIterator beginIterator;

    public:
        explicit ChildProxy(IEntity *firstChild);

        [[nodiscard]] ChildListIterator begin() const noexcept;
        [[nodiscard]] ChildListIterator end() const noexcept;
    };

    class IEntity : public ISerialize {
        friend class EntityManager;

    private:
        void recSetActive() const;
        void onDelete();

    protected:
        ComponentManager *componentManagerInstance;
        EntityId entityId;
        EntityId parentId = INVALID_ENTITY_ID;
        EntityId leftChildId = INVALID_ENTITY_ID;
        EntityId rightEntityId = INVALID_ENTITY_ID;

        u32 childCount = 0;

        u8 localActive: 1;
        u8 globalActive: 1;
        u8 destroyed: 1;
        u8 reserved: 5;

        virtual void OnDelete() {}

    public:
        IEntity();
        virtual ~IEntity() noexcept = default;

        template<class T>
        T *GetComponent() const
        {
            return componentManagerInstance->template GetComponent<T>(entityId);
        }

        template<class T, class ...P>
        T *AddComponent(P &&...param)
        {
            return componentManagerInstance->template AddComponent<T>(entityId, std::forward<P>(param)...);
        }

        template<class T>
        void RemoveComponent()
        {
            componentManagerInstance->template RemoveComponent<T>(entityId);
        }

        inline bool operator==(const IEntity &rhs) const { return entityId == rhs.entityId; }
        inline bool operator!=(const IEntity &rhs) const { return entityId != rhs.entityId; }
        inline bool operator==(const IEntity *rhs) const { return entityId == rhs->entityId; }
        inline bool operator!=(const IEntity *rhs) const { return entityId != rhs->entityId; }

        [[nodiscard]] virtual const EntityTypeId GetStaticEntityTypeId() const = 0;

        [[nodiscard]] inline EntityId GetEntityId() const noexcept { return entityId; }
        [[nodiscard]] inline EntityId GetParentId() const noexcept { return parentId; }

        [[nodiscard]] IEntity *GetParent() const noexcept;

        void SetParent(IEntity *entity);
        void SetParent(EntityId newParentId);

        [[nodiscard]] inline EntityId GetChildId() const noexcept { return leftChildId; }
        [[nodiscard]] inline EntityId GetNextParentChildId() const noexcept { return rightEntityId; }

        [[nodiscard]] IEntity *GetChild() const;
        [[nodiscard]] IEntity *GetNextParentChild() const;

        [[nodiscard]] ChildProxy GetChildIterable() const noexcept;

        [[nodiscard]] inline u32 GetChildCount() const noexcept { return childCount; }

        void AddChild(EntityId newChildId);
        void AddChild(IEntity *entity);

        void RemoveChild(EntityId childId);

        void SetActive(bool active);

        [[nodiscard]] inline bool IsLocalActive() const noexcept { return localActive; }
        [[nodiscard]] inline bool IsActive() const noexcept { return localActive && globalActive; }

        [[nodiscard]] json SerializeComponents() const;
        void UnSerializeComponents(const json &j);

        [[nodiscard]] json SerializeObj() const override;
        void UnSerializeObj(const json &j) override;

        virtual void OnEnable() {}
        virtual void OnDisable() {}
    };
}

#endif
