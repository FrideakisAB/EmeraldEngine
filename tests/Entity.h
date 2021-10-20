#include "ECS/Entity.h"
#include "Engine/Engine.h"
#include "ECS/Component.h"
#include "ECS/EntityManager.h"
#include "ECS/Serialization/EntityFactory.h"
#include "ECS/Serialization/ComponentFactory.h"
#include <set>

TEST_CASE("Entity subsystem test", "[IAllocator]")
{
    auto &EM = Engine::Get().GetEntityManager();

    SECTION("Base activity test")
    {
        class TestEntity final : public ECS::Entity<TestEntity> {
        public:
            TestEntity() {}
        };

        auto *entity = EM.GetEntity(EM.CreateEntity<TestEntity>());

        REQUIRE( entity->IsActive() == true );
        REQUIRE( entity->IsLocalActive() == true );

        entity->SetActive(false);
        entity->SetActive(false);

        REQUIRE( entity->IsActive() == false );
        REQUIRE( entity->IsLocalActive() == false );

        entity->SetActive(true);
        entity->SetActive(true);

        REQUIRE( entity->IsActive() == true );
        REQUIRE( entity->IsLocalActive() == true );

        EM.DestroyEntity(entity->GetEntityId());
    }

    SECTION("Creation and delete Entity objects")
    {
        static bool isDelete = false;

        class TestEntity final : public ECS::Entity<TestEntity> {
        protected:
            void OnDelete() final
            {
                isDelete = true;
            }

        public:
            TestEntity() {}
        };

        REQUIRE( TestEntity::STATIC_ENTITY_TYPE_ID != 0 );

        auto *entity = EM.GetEntity(EM.CreateEntity<TestEntity>());

        delete (TestEntity*)entity;

        REQUIRE( entity->GetStaticEntityTypeId() != 0 );
        REQUIRE( entity->GetEntityId() != ECS::INVALID_ENTITY_ID );

        EM.DestroyEntity(entity->GetEntityId());

        REQUIRE( isDelete == true );

        EM.DestroyEntity(ECS::INVALID_ENTITY_ID);

        EM.RemoveDestroyedEntities();
    }

    SECTION("Scene graph test")
    {
        class TestEntity final : public ECS::Entity<TestEntity> {
        public:
            TestEntity() {}
        };

        auto *entity = EM.GetEntity(EM.CreateEntity<TestEntity>());
        auto *entity2 = EM.GetEntity(EM.CreateEntity<TestEntity>());

        REQUIRE( entity->GetParentId() == ECS::INVALID_ENTITY_ID );
        REQUIRE( entity2->GetParentId() == ECS::INVALID_ENTITY_ID );

        entity->AddChild(entity2);

        REQUIRE( entity2->GetParentId() == entity->GetEntityId() );
        REQUIRE( entity2->GetParent() == entity );
        REQUIRE( entity->GetChildId() == entity2->GetEntityId() );
        REQUIRE( entity->GetChild() == entity2 );
        REQUIRE( entity2->GetNextParentChildId() == ECS::INVALID_ENTITY_ID );

        for (auto &ent : entity->GetChildIterable())
            REQUIRE( ent.GetEntityId() == entity2->GetEntityId() );

        entity2->SetParent(ECS::INVALID_ENTITY_ID);

        REQUIRE( entity2->GetParentId() == ECS::INVALID_ENTITY_ID );
        REQUIRE( entity->GetChildId() == ECS::INVALID_ENTITY_ID );

        entity2->SetParent(entity);

        REQUIRE( entity2->GetParentId() == entity->GetEntityId() );

        auto *entity3 = EM.GetEntity(EM.CreateEntity<TestEntity>());

        entity3->SetParent(entity->GetEntityId());

        std::set<ECS::EntityId> origIds { entity2->GetEntityId(), entity3->GetEntityId() };

        std::set<ECS::EntityId> entIds;
        for (auto &ent : entity->GetChildIterable())
            entIds.insert(ent.GetEntityId());

        REQUIRE( origIds == entIds );

        entity3->SetParent(nullptr);

        REQUIRE( entity3->GetParentId() == ECS::INVALID_ENTITY_ID );

        entity->AddChild(entity3);

        auto *entity4 = EM.GetEntity(EM.CreateEntity<TestEntity>());
        entity->AddChild(entity4);

        REQUIRE( entity3->GetParentId() == entity->GetEntityId() );

        entity->RemoveChild(entity2->GetEntityId());

        REQUIRE( entity2->GetParentId() == ECS::INVALID_ENTITY_ID );

        entity3->AddChild(entity2->GetEntityId());

        REQUIRE( entity2->GetParentId() == entity3->GetEntityId() );

        EM.DestroyEntity(entity->GetEntityId());
        EM.DestroyEntity(entity2->GetEntityId());
        EM.DestroyEntity(entity3->GetEntityId());
        EM.DestroyEntity(entity4->GetEntityId());
    }

    SECTION("Activity in graph test")
    {
        class TestEntity final : public ECS::Entity<TestEntity> {
        public:
            TestEntity() {}
        };

        auto *entity = EM.GetEntity(EM.CreateEntity<TestEntity>());
        auto *entity2 = EM.GetEntity(EM.CreateEntity<TestEntity>());

        entity->AddChild(entity2);

        entity->SetActive(false);

        REQUIRE( entity->IsActive() == false );
        REQUIRE( entity->IsLocalActive() == false );
        REQUIRE( entity2->IsActive() == false );
        REQUIRE( entity2->IsLocalActive() == true );

        entity->SetActive(true);
        entity2->SetActive(false);

        REQUIRE( entity->IsActive() == true );
        REQUIRE( entity->IsLocalActive() == true );
        REQUIRE( entity2->IsActive() == false );
        REQUIRE( entity2->IsLocalActive() == false );

        EM.DestroyEntity(entity->GetEntityId());
        EM.DestroyEntity(entity2->GetEntityId());
    }

    SECTION("Serialize & components test")
    {
        class TestEntity final : public ECS::Entity<TestEntity> {
        public:
            TestEntity() {}
        };

        class TestComponent final : public ECS::Component<TestComponent> {
        public:
            TestComponent() {}
        };

        Engine::Get().GetEntityFactory().Register<TestEntity>();
        Engine::Get().GetComponentFactory().Register<TestComponent>();

        auto *entity = EM.GetEntity(EM.CreateEntity<TestEntity>());
        auto *entity2 = EM.GetEntity(EM.CreateEntity<TestEntity>());

        entity->AddComponent<TestComponent>();
        entity->AddChild(entity2);

        REQUIRE( entity->GetComponent<TestComponent>() != nullptr );

        entity->RemoveComponent<TestComponent>();

        REQUIRE( entity->GetComponent<TestComponent>() == nullptr );

        entity->AddComponent<TestComponent>();

        json serializeObject = entity->SerializeObj();

        REQUIRE( serializeObject.empty() == false );

        json serializeComponents = entity->SerializeComponents();

        REQUIRE( serializeComponents.empty() == false );

        entity->UnSerializeComponents(serializeComponents);

        auto *entity3 = EM.GetEntity(EM.CreateEntity<TestEntity>());

        entity3->UnSerializeObj(serializeObject);

        REQUIRE( entity3->GetComponent<TestComponent>() != nullptr );
        REQUIRE( entity3->GetChildId() != ECS::INVALID_ENTITY_ID );

        EM.DestroyEntity(entity->GetEntityId());
        EM.DestroyEntity(entity2->GetEntityId());
        EM.DestroyEntity(entity3->GetEntityId());
    }
}
