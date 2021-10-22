#ifndef ENGINE_H
#define ENGINE_H

#include "ECS/API.h"
#include "Utils/Singleton.h"
#include "ECS/Event/EventHandler.h"
#include "ECS/Event/EventDelegate.h"

namespace ECS {
    class IEntity;
    class IComponent;
    class ISystem;

    namespace Utils {
        class Timer;
        namespace Internal {
            template<class T>
            class FamilyTypeID;
        }
    }

    namespace Event {
        class IEvent;
        class IEventListener;
    }

    class EntityManager;
    class SystemManager;
    class ComponentManager;
    class ComponentFactory;
    class EntityFactory;
}

class Engine : public Singleton<Engine> {
    friend Singleton<Engine>;
    friend ECS::Event::IEventListener;

private:
    ECS::Utils::Timer *engineTime;
    ECS::EntityManager *entityManager;
    ECS::ComponentManager *componentManager;
    ECS::SystemManager *systemManager;
    ECS::Event::EventHandler *eventHandler;

    ECS::ComponentFactory *componentFactory;
    ECS::EntityFactory *entityFactory;

    Engine();
    ~Engine();

    template<class E>
    inline void SubscribeEvent(ECS::Event::Internal::IEventDelegate *const eventDelegate)
    {
        eventHandler->AddEventCallback<E>(eventDelegate);
    }

    void UnsubscribeEvent(ECS::Event::Internal::IEventDelegate *eventDelegate);

public:
    template<class E, class... ARGS>
    void SendEvent(ARGS&&... eventArgs)
    {
        eventHandler->Send<E>(std::forward<ARGS>(eventArgs)...);
    }

    void Update();

    [[nodiscard]] ECS::EntityManager &GetEntityManager() const noexcept { return *entityManager; }
    [[nodiscard]] ECS::ComponentManager &GetComponentManager() const noexcept { return *componentManager; }
    [[nodiscard]] ECS::SystemManager &GetSystemManager() const noexcept { return *systemManager; }
    [[nodiscard]] ECS::ComponentFactory &GetComponentFactory() const noexcept { return *componentFactory; }
    [[nodiscard]] ECS::EntityFactory &GetEntityFactory() const noexcept { return *entityFactory; }
    [[nodiscard]] ECS::Utils::Timer &GetTimer() const noexcept { return *engineTime; }
};

#endif
