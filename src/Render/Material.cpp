#include "Render/Material.h"

IAsset *Material::Clone() const
{
    return new Material(std::cref(*this));
}

json Material::SerializeObj() const
{
    json data;

    data["nameType"] = boost::typeindex::type_id<Material>().pretty_name();

    data["shaderName"] = Shader;
    data["uniformsMap"] = {};
    data["samplersMap"] = {};

    for (const auto&[name, param]: Uniforms)
    {
        json paramData;
        ns::to_json(paramData, param);
        data["uniformsMap"].emplace_back(json{name, paramData});
    }

    for (const auto&[name, handle]: Samplers)
    {
        if (!handle->GetName().empty())
            data["samplersMap"].emplace_back(json{name, handle->GetName()});
    }

    return data;
}

void Material::UnSerializeObj(const json &j)
{
    Shader = j["shaderName"];

    for (auto &elm: j["uniformsMap"].items())
    {
        Shaders::ShaderParamValue param;
        ns::from_json(elm.value()[1], param);
        Uniforms[elm.value()[0].get<std::string>()] = param;
    }

    //TODO: sampler unserialize
}
