#include "ECS/IComponent.h"

namespace ECS {
    IComponent::IComponent() :
            owner(INVALID_ENTITY_ID), enabled(true), treeLock(false)
    {}

    void IComponent::SetActive(bool active)
    {
        if (enabled == active)
            return;

        if (!active)
            OnDisable();
        else
            OnEnable();

        enabled = active;
    }
}
