#include "ECS/System.h"
#include "ECS/Utils/Timer.h"
#include "ECS/SystemManager.h"

TEST_CASE( "System subsystem test", "[System]" )
{
    logger = new Log();

    auto &SM = Engine::Get().GetSystemManager();

    static f32 timeUpdate1, timeUpdate2;

    class TestSystem : public ECS::System<TestSystem> {
    public:
        int i = 0;

        void Update() final { i++; timeUpdate1 = Engine::Get().GetTimer().GetWorkTime(); }
    };

    class TestSystem2 : public ECS::System<TestSystem2> {
    public:
        int i = 0;

        void Update() final { i++; timeUpdate2 = Engine::Get().GetTimer().GetWorkTime(); }
    };

    class TestSystem3 : public ECS::System<TestSystem2> {
    public:
        int i = 0;
    };

    SECTION("System manager test")
    {
        SM.DisableSystem<TestSystem>();
        SM.EnableSystem<TestSystem>();

        SM.SetSystemPriority<TestSystem>(ECS::HIGHEST_SYSTEM_PRIORITY);
        SM.SetSystemUpdateInterval<TestSystem>(1.0f);

        SM.AddSystem<TestSystem>();
        auto *system = SM.GetSystem<TestSystem>();
        SM.AddSystem<TestSystem2>();
        SM.GetSystem<TestSystem2>()->AddDependencies(system);
        SM.AddSystem<TestSystem3>();

        REQUIRE( system != nullptr );
        REQUIRE( system == SM.AddSystem<TestSystem>() );

        SM.DisableSystem<TestSystem>();
        SM.DisableSystem<TestSystem2>();
        SM.DisableSystem<TestSystem3>();

        for (bool active : SM.GetSystemWorkState())
            REQUIRE( active == false );

        SM.EnableSystem<TestSystem>();
        SM.EnableSystem<TestSystem2>();
        SM.EnableSystem<TestSystem3>();

        for (bool active : SM.GetSystemWorkState())
            REQUIRE( active == true );

        SM.SetSystemWorkState(SM.GetSystemWorkState());

        for (bool active : SM.GetSystemWorkState())
            REQUIRE( active == true );

        SM.SetSystemPriority<TestSystem>(ECS::HIGHEST_SYSTEM_PRIORITY);
        SM.SetSystemUpdateInterval<TestSystem2>(-1.0f);

        Engine::Get().Update();

        REQUIRE( system->i == 1 );
        REQUIRE( timeUpdate1 <= timeUpdate2 );
    }

    SECTION("System interface test")
    {
        auto *system = SM.GetSystem<TestSystem>();

        system->PreUpdate();
        system->Update();
        system->PostUpdate();

        REQUIRE( system->GetStaticSystemTypeId() == TestSystem::STATIC_SYSTEM_TYPE_ID );
    }

    delete logger;
}
