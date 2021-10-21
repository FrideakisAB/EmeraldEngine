#include "ECS/Component.h"
#include "ECS/ComponentManager.h"

TEST_CASE( "Components subsystem test", "[Component]" )
{
    auto &CM = Engine::Get().GetComponentManager();

    class TestEntity : public ECS::Entity<TestEntity> {};

    ECS::EntityId entityId = Engine::Get().GetEntityManager().CreateEntity<TestEntity>();

    SECTION("ComponentManager work test")
    {
        class TestComponent : public ECS::Component<TestComponent> {};

        REQUIRE( CM.AddComponent<TestComponent>(entityId) != nullptr );

        auto *component = CM.GetComponent<TestComponent>(entityId);
        REQUIRE( component != nullptr );
        REQUIRE( component == CM.AddComponent<TestComponent>(entityId) );

        CM.RemoveComponent<TestComponent>(entityId);

        REQUIRE( CM.GetComponent<TestComponent>(entityId) == nullptr );

        CM.RemoveComponent<TestComponent>(entityId);

        CM.AddComponent<TestComponent>(entityId);
        ECS::ComponentId cmpId = CM.GetComponent<TestComponent>(entityId)->GetComponentId();
        CM.RemoveComponent(entityId, cmpId);

        REQUIRE( CM.GetComponent<TestComponent>(entityId) == nullptr );

        CM.RemoveComponent(entityId, cmpId);

        CM.AddComponent<TestComponent>(entityId);
        CM.RemoveAllComponents(entityId);

        REQUIRE( CM.GetComponent<TestComponent>(entityId) == nullptr );

        CM.AddComponent<TestComponent>(entityId);

        for (auto &localComponent : CM.GetIterable<TestComponent>())
            REQUIRE( localComponent.GetComponentId() == CM.GetComponent<TestComponent>(entityId)->GetComponentId() );

        CM.RemoveComponent(entityId, ECS::INVALID_COMPONENT_ID);

        REQUIRE( CM.GetComponent<TestComponent>(ECS::INVALID_ENTITY_ID) == nullptr );
    }

    SECTION("Component work test")
    {
        ECS::EntityId entity2Id = Engine::Get().GetEntityManager().CreateEntity<TestEntity>();

        static bool isTreeUpdate = false;
        static int enableCount = 0;

        class TestComponent : public ECS::Component<TestComponent> {
        protected:
            void UpdateTree() final
            {
                isTreeUpdate = true;
            }

            void OnEnable() final
            {
                ++enableCount;
            }

        public:
            TestComponent() { treeLock = true; }
        };

        auto *component = CM.AddComponent<TestComponent>(entityId);
        Engine::Get().GetEntityManager().GetEntity(entityId)->SetParent(entity2Id);

        REQUIRE( component->IsActive() == true );
        REQUIRE( component->GetStaticComponentTypeId() == TestComponent::STATIC_COMPONENT_TYPE_ID );
        REQUIRE( component->GetOwner() == entityId );
        REQUIRE( component->GetComponentId() != ECS::INVALID_COMPONENT_ID );

        component->SetActive(false);

        REQUIRE( component->IsActive() == false );

        component->SetActive(true);
        component->SetActive(true);

        REQUIRE( enableCount == 1 );
        REQUIRE( component->IsActive() == true );

        Engine::Get().GetEntityManager().GetEntity(entityId)->SetParent(ECS::INVALID_ENTITY_ID);

        REQUIRE( isTreeUpdate == true );

        Engine::Get().GetEntityManager().GetEntity(entity2Id)->SetParent(entityId);
    }

    Engine::Get().GetEntityManager().DestroyEntity(entityId);
    Engine::Get().GetEntityManager().RemoveDestroyedEntities();
}
