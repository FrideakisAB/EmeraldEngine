#ifndef MATERIALCOMPONENT_H
#define MATERIALCOMPONENT_H

#include "ECS/Component.h"
#include "Render/Material.h"

class MaterialComponent final : public ECS::Component<MaterialComponent> {
private:
    AssetsHandle materialHandle;

public:
    void SetMaterial(const AssetsHandle &asset);
    [[nodiscard]] Material *GetMaterial() const noexcept { return static_cast<Material*>(materialHandle.get()); }
    [[nodiscard]] AssetsHandle GetMaterialHandle() const noexcept { return materialHandle; }
    [[nodiscard]] bool IsValid() const noexcept { return materialHandle.get() != nullptr; }
};

#endif
