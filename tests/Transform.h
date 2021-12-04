#include "Engine/Components/Transform.h"

TEST_CASE( "Transform work test", "[Transform]" )
{
    class TestEntity : public ECS::Entity<TestEntity> {};

    auto &EM = Engine::Get().GetEntityManager();

    SECTION("Transform base test")
    {
        auto *entity = EM.GetEntity(EM.CreateEntity<TestEntity>());

        auto *transform = entity->AddComponent<Transform>();

        REQUIRE( transform->GetPositionType() == PositionType::Local );

        Position pos = transform->GetLocalPosition();

        REQUIRE( pos.position == glm::vec3(0.0f) );
        REQUIRE( pos.rotate == glm::quat() );
        REQUIRE( pos.scale == glm::vec3(1.0f) );

        pos.position = glm::vec3(1.0f, 3.0f, 0.0f);

        transform->SetLocalPosition(pos);
        transform->SetPositionType(PositionType::Global);

        REQUIRE( transform->GetPositionType() == PositionType::Global );
        REQUIRE( transform->GetGlobalPosition().position == glm::vec3(1.0f, 3.0f, 0.0f) );

        EM.DestroyEntity(entity->GetEntityId());
    }
}
