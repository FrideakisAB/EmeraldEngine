#include "Utils/Singleton.h"

TEST_CASE("Singleton is work correct", "[Singleton]")
{
    SECTION("Singleton not derived test")
    {
        Singleton<float>::Get() = 9.7f;
        REQUIRE( Singleton<float>::Get() == 9.7f );

        Singleton<float>::Get() = 5.5f;
        REQUIRE( Singleton<float>::Get() == 5.5f );

        REQUIRE( Singleton<float>::Get() == Singleton<float>::Get() );
        REQUIRE( &Singleton<float>::Get() == &Singleton<float>::Get() );

        Singleton<float>::Destroy();
    }

    SECTION("Singleton derived test")
    {
        static bool deleteFlag = false;

        class SingleObject : public Singleton<SingleObject> {
        public:
            int x = 0;

            ~SingleObject() { deleteFlag = true; }
        };

        REQUIRE( SingleObject::Get().x == 0 );
        REQUIRE( Singleton<SingleObject>::Get().x == 0 );

        SingleObject::Get().x = 9;
        REQUIRE( SingleObject::Get().x == 9 );
        REQUIRE( Singleton<SingleObject>::Get().x == 9 );

        REQUIRE( &SingleObject::Get() == &Singleton<SingleObject>::Get() );

        SingleObject::Destroy();
        Singleton<SingleObject>::Destroy();
        REQUIRE( deleteFlag == true );
    }
}
