#ifndef MATERIAL_H
#define MATERIAL_H

#include "Utils/glm.h"
#include "Assets/IAsset.h"
#include <boost/type_index.hpp>
#include "Render/ShaderTools.h"

class Material final : public IAsset {
public:
    std::string Shader;
    std::map<std::string, Shaders::ShaderParamValue> Uniforms;
    std::map<std::string, AssetsHandle> Samplers;

    [[nodiscard]] size_t GetTypeId() const noexcept final
    {
        return boost::typeindex::type_id<Material>().hash_code();
    }

    [[nodiscard]] IAsset *Clone() const final;

    [[nodiscard]] json SerializeObj() const final;
    void UnSerializeObj(const json& j) final;
};

#endif
