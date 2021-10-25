#ifndef IASSET_H
#define IASSET_H

#include <string>
#include "ECS/Serialization/ISerialize.h"
#include "ECS/Serialization/IBinarySerialize.h"

class IAsset : public ISerialize, public IBinarySerialize {
private:
    bool dynamic = true;
    std::string name;

protected:
    explicit IAsset(bool binaryNeeded = false) noexcept: IBinarySerialize(binaryNeeded) {}
    IAsset(const IAsset &asset) noexcept: IBinarySerialize(asset.IsBinaryNeeded()) {}
    IAsset(IAsset &&asset) noexcept: IBinarySerialize(std::move(asset))
    {
        std::swap(asset.name, name);
        std::swap(asset.dynamic, dynamic);
    }

public:
    virtual ~IAsset() = default;

    [[nodiscard]] virtual IAsset *Clone() const { return nullptr; }

    [[nodiscard]] bool IsDynamic() const noexcept { return dynamic; }
    [[nodiscard]] virtual size_t GetTypeId() const noexcept = 0;
    [[nodiscard]] const std::string &GetName() const noexcept { return name; }
};

using AssetsHandle = std::shared_ptr<IAsset>;

#endif
