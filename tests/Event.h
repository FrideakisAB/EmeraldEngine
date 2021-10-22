#include "ECS/Event/Event.h"
#include "ECS/Event/IEventListener.h"

TEST_CASE( "Event system work test", "[Event]" )
{
    auto &engine = Engine::Get();

    SECTION("Integration event test")
    {
        struct TestEvent : public ECS::Event::Event<TestEvent> {
        public:
            int i = 0;

            explicit TestEvent(int i) : i(i) {}
        };

        static bool isEventAccepted = false;

        class TestListener : protected ECS::Event::IEventListener {
        private:
            void OnTestEvent(const TestEvent *const event)
            {
                REQUIRE( event->GetEventTypeId() == TestEvent::STATIC_EVENT_TYPE_ID );
                REQUIRE( event->GetEventTypeId() != ECS::Event::INVALID_EVENT_TYPE );
                REQUIRE( event->GetTimeCreated() == 0 );
                REQUIRE( event->i == 11 );

                isEventAccepted = true;
            }

        public:
            TestListener()
            {
                RegisterEventCallback<TestEvent, TestListener>(&TestListener::OnTestEvent);
            }

            ~TestListener() override
            {
                UnregisterAllEventCallbacks();
            }
        };

        TestListener listener{};

        engine.SendEvent<TestEvent>(11);

        REQUIRE( isEventAccepted == false );

        engine.Update();

        REQUIRE( isEventAccepted == true );
    }

    SECTION("Event handler test")
    {
        struct TestEvent : public ECS::Event::Event<TestEvent> {};

        static bool isEventAccepted = false;

        class TestListener : public ECS::Event::IEventListener {
        public:
            void OnTest(const TestEvent *const event) { isEventAccepted = true; }
        };

        TestListener listener;

        listener.RegisterEventCallback<TestEvent, TestListener>(&TestListener::OnTest);

        engine.SendEvent<TestEvent>();
        engine.Update();

        REQUIRE( isEventAccepted == true );
        isEventAccepted = false;

        listener.UnregisterEventCallback(&TestListener::OnTest);
        listener.RegisterEventCallback<TestEvent, TestListener>(&TestListener::OnTest);

        engine.SendEvent<TestEvent>();
        engine.Update();

        REQUIRE( isEventAccepted == true );
        isEventAccepted = false;

        listener.RegisterEventCallback<TestEvent, TestListener>(&TestListener::OnTest);
        listener.UnregisterAllEventCallbacks();

        engine.SendEvent<TestEvent>();
        engine.Update();

        REQUIRE( isEventAccepted == false );
    }
}
