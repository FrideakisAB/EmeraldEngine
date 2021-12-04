#include "Engine/Components/MaterialComponent.h"

void MaterialComponent::SetMaterial(const AssetsHandle &asset)
{
    if (dynamic_cast<Material*>(asset.get()) != nullptr)
        materialHandle = asset;
}
