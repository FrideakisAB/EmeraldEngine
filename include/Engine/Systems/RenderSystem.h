#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "ECS/System.h"

class RenderSystem final : public ECS::System<RenderSystem> {
public:
    RenderSystem();
    ~RenderSystem() final;

    void PreUpdate() final;
    void Update() final;
    void PostUpdate() final;
};

#endif
