#include "ECS/Utils/Timer.h"
#include "Engine/Engine.h"
#include <thread>

using namespace std::chrono_literals;

TEST_CASE( "Timer test", "[Timer]" )
{
    SECTION("Base work test")
    {
        Engine::Get().Update();

        std::this_thread::sleep_for(16ms);

        Engine::Get().Update();

        auto &timer = Engine::Get().GetTimer();

        REQUIRE( timer.GetTimeStamp().asFloat == timer.GetWorkTime() );

        timer.SetTimeScale(2.0f);

        REQUIRE( timer.GetTimeScale() == 2.0f );
        REQUIRE( timer.GetDeltaTime() == timer.GetNonScaleDeltaTime() );

        timer.Reset();

        REQUIRE( timer.GetWorkTime() == 0 );
    }
}

TEST_CASE( "JSON utils test", "[JSON]" )
{
    logger = new Log();

    SECTION("Parse test")
    {
        REQUIRE_NOTHROW( JsonUtils::TryParse("bb1{22)").empty() == true );
    }

    delete logger;
}
