#include "Engine/Systems/RenderSystem.h"

RenderSystem::RenderSystem() : System(ECS::VERY_LOW_SYSTEM_PRIORITY)
{}

RenderSystem::~RenderSystem() = default;

void RenderSystem::PreUpdate()
{
    System::PreUpdate();
}

void RenderSystem::Update()
{
    System::Update();
}

void RenderSystem::PostUpdate()
{
    System::PostUpdate();
}
