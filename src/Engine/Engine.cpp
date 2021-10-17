#include "Engine/Engine.h"

#include "ECS/Utils/Timer.h"
#include "ECS/SystemManager.h"
#include "ECS/EntityManager.h"
#include "ECS/ComponentManager.h"
#include "ECS/Utils/FamilyTypeID.h"
#include "ECS/Serialization/EntityFactory.h"
#include "ECS/Serialization/ComponentFactory.h"

Engine::Engine()
{
    engineTime = new ECS::Utils::Timer();
    eventHandler = new ECS::Event::EventHandler();
    systemManager = new ECS::SystemManager();
    componentManager = new ECS::ComponentManager();
    entityManager = new ECS::EntityManager(componentManager);
    componentFactory = new ECS::ComponentFactory();
    entityFactory = new ECS::EntityFactory();
}

Engine::~Engine()
{
    delete entityManager;
    delete engineTime;
    delete componentManager;
    delete systemManager;
    delete eventHandler;
    delete componentFactory;
    delete entityFactory;
}

void Engine::Update()
{
    entityManager->RemoveDestroyedEntities();

    engineTime->Tick();
    systemManager->Update();
    eventHandler->DispatchEvents();
}

void Engine::UnsubscribeEvent(ECS::Event::Internal::IEventDelegate *eventDelegate)
{
    eventHandler->RemoveEventCallback(eventDelegate);
}
